/**
 * @file shellscribe.c
 * @brief Main implementation of shellscribe functionality
 * 
 * This file contains the core functions for parsing shell scripts and 
 * generating documentation, including file handling and directory operations.
 * It provides the high-level API for the ShellScribe documentation generator,
 * coordinating the parsing of shell scripts and the rendering of documentation
 * into various output formats.
 * 
 * @see parser_engine.h
 * @see renderer_engine.h
 */

#include "core/shellscribe.h"
#include "parsers/parser_engine.h"
#include "renderers/renderer_engine.h"
#include "utils/debug.h"
#include "utils/memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>

// If PATH_MAX is not defined, define it with a reasonable value
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/**
 * @brief Maximum number of documentation blocks supported per processing
 *
 * This constant defines the upper limit on the number of documentation blocks
 * that can be extracted from a single shell script file. This limit helps prevent
 * memory allocation issues when processing large scripts.
 */
#define MAX_DOCBLOCKS 1000

/**
 * @brief Reduced block allocation for memory-efficient processing
 *
 * This constant defines a smaller allocation size used for more memory-efficient
 * processing in situations where we know a file will have fewer documentation blocks.
 * Using this smaller allocation can reduce memory usage significantly when processing
 * many small files.
 */
#define REDUCED_DOCBLOCKS 100

/**
 * @brief Parse a shell script and return the documentation blocks
 * 
 * This function processes a shell script file, extracts all documentation blocks,
 * and returns them as an array of shellscribe_docblock_t structures. It is the
 * main entry point for the documentation generation process.
 * 
 * @param file_path Path to the shell script file to parse
 * @param block_count Output parameter to store the number of documentation blocks found
 * @param config Configuration options controlling parsing behavior
 * 
 * @return shellscribe_docblock_t* Array of documentation blocks, or NULL on error.
 *         The caller is responsible for freeing this memory using free_docblocks().
 * 
 * @note If any parameter is NULL, the function returns NULL
 * @note If no documentation blocks are found or an error occurs during parsing,
 *       the function returns NULL and prints an error message
 * @note Memory is allocated for up to MAX_DOCBLOCKS documentation blocks
 * @note The actual number of blocks found is stored in the block_count parameter
 * 
 * @see parse_shell_file
 * @see free_docblocks
 * @see MAX_DOCBLOCKS
 */
shellscribe_docblock_t* parse_shell_script(const char *file_path, int *block_count, const shellscribe_config_t *config) {
    if (file_path == NULL || block_count == NULL || config == NULL) {
        return NULL;
    }
    shellscribe_docblock_t *docblocks = (shellscribe_docblock_t *)shell_calloc(MAX_DOCBLOCKS, sizeof(shellscribe_docblock_t));
    if (docblocks == NULL) {
        fprintf(stderr, "Error: unable to allocate memory for documentation blocks\n");
        return NULL;
    }
    int count = parse_shell_file(file_path, config, docblocks, MAX_DOCBLOCKS);
    if (count <= 0) {
        fprintf(stderr, "Error: unable to parse file %s\n", file_path);
        for (int i = 0; i < MAX_DOCBLOCKS; i++) {
            free_docblock(&docblocks[i]);
        }
        void *docblocks_ptr = docblocks;
        shell_free(&docblocks_ptr);
        return NULL;
    }
    
    *block_count = count;
    return docblocks;
}

/**
 * @brief Creates a directory and all parent directories if needed
 * 
 * Recursively creates directories along the given path, similar to `mkdir -p`.
 * If any part of the path already exists as a non-directory, the function fails.
 * This function is used to ensure that all necessary directories exist before
 * writing output files.
 * 
 * @param path The directory path to create
 * 
 * @return bool true if all directories were created or already exist,
 *              false if an error occurred
 * 
 * @note This function modifies the provided path string temporarily during processing
 * @note If a component of the path exists but is not a directory, the function
 *       fails and returns false
 * @note The function creates directories with permission mode 0755 (rwxr-xr-x)
 * @note Error messages are printed to stderr in case of failure
 */
static bool create_directories_recursive(const char *path) {
    char tmp[PATH_MAX];
    char *p = NULL;
    size_t len;
    strncpy(tmp, path, PATH_MAX - 1);
    tmp[PATH_MAX - 1] = '\0';
    len = strlen(tmp);
    if (tmp[len - 1] == '/') {
        tmp[len - 1] = '\0';
    }
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            struct stat st;
            if (stat(tmp, &st) != 0) {
                if (mkdir(tmp, 0755) != 0) {
                    fprintf(stderr, "Error creating directory %s: %s\n", tmp, strerror(errno));
                    return false;
                }
            } else if (!S_ISDIR(st.st_mode)) {
                fprintf(stderr, "Error: %s exists but is not a directory\n", tmp);
                return false;
            }
            *p = '/';
        }
    }
    struct stat st;
    if (stat(tmp, &st) != 0) {
        if (mkdir(tmp, 0755) != 0) {
            fprintf(stderr, "Error creating directory %s: %s\n", tmp, strerror(errno));
            return false;
        }
    } else if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "Error: %s exists but is not a directory\n", tmp);
        return false;
    }
    return true;
}

/**
 * @brief Get output file path by preserving the directory structure
 * 
 * Constructs an output file path for the documentation file, preserving the
 * directory structure of the input file relative to the base directory.
 * The function also ensures that all required directories in the path exist.
 * 
 * @param doc_path Base output directory for documentation
 * @param input_file Path to the input shell script file
 * @param base_dir Base directory to calculate relative paths from (can be NULL)
 * 
 * @return char* Newly allocated string containing the output path (caller must free),
 *               or NULL if memory allocation fails or directory creation fails
 * 
 * @note The returned path must be freed by the caller using shell_free()
 * @note This function creates all necessary directories in the output path
 * @note If base_dir is provided, the relative path from base_dir to input_file
 *       is preserved in the output path
 * @note If base_dir is NULL or empty, only the filename is used
 * @note The output file always has a .md extension, regardless of the input file extension
 * 
 * @see create_directories_recursive
 */
static char *get_output_path(const char *doc_path, const char *input_file, const char *base_dir) {
    size_t dir_len = strlen(doc_path);
    size_t input_len = strlen(input_file);
    size_t base_dir_len = base_dir ? strlen(base_dir) : 0;
    size_t md_ext_len = 3; // ".md"
    char *output_path = shell_malloc(dir_len + input_len + md_ext_len + 10);
    if (output_path == NULL) {
        return NULL;
    }
    strcpy(output_path, doc_path);
    if (doc_path[dir_len - 1] != '/') {
        strcat(output_path, "/");
    }
    const char *relative_path = NULL;
    if (base_dir && base_dir_len > 0 && strncmp(input_file, base_dir, base_dir_len) == 0) {
        relative_path = input_file + base_dir_len;
        while (*relative_path == '/') {
            relative_path++;
        }
    } else {
        relative_path = strrchr(input_file, '/');
        relative_path = relative_path ? relative_path + 1 : input_file;
    }
    char *rel_path_copy = shell_strdup(relative_path);
    if (!rel_path_copy) {
        return NULL;
    }
    char *last_slash = strrchr(rel_path_copy, '/');
    if (last_slash) {
        *last_slash = '\0';
        strcat(output_path, rel_path_copy);
        strcat(output_path, "/");
        if (!create_directories_recursive(output_path)) {
            shell_free((void **)&rel_path_copy);
            return NULL;
        }
        relative_path = last_slash + 1;
    }
    char *basename = shell_strdup(relative_path);
    if (!basename) {
        shell_free((void **)&rel_path_copy);
        return NULL;
    }
    char *dot = strrchr(basename, '.');
    if (dot) {
        *dot = '\0';
    }
    strcat(output_path, basename);
    strcat(output_path, ".md");
    shell_free((void **)&rel_path_copy);
    shell_free((void **)&basename);

    return output_path;
}

/**
 * @brief Ensure the output directory exists
 * 
 * Checks if the specified directory exists and is a directory.
 * If it doesn't exist, attempts to create it with standard permissions.
 * This function is used to ensure that the output directory for documentation
 * exists before attempting to write files.
 * 
 * @param dir_path Path to the directory to check or create
 * 
 * @return bool true if the directory exists or was created successfully,
 *              false if an error occurred or the path exists but is not a directory
 * 
 * @note If the directory already exists, the function verifies that it is actually
 *       a directory and not a regular file or other file type
 * @note The function creates directories with permission mode 0755 (rwxr-xr-x)
 * @note Error messages are printed to stderr in case of failure
 */
static bool ensure_directory(const char *dir_path) {
    struct stat st;
    if (stat(dir_path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            return true;
        }
        return false;
    }
    if (mkdir(dir_path, 0755) != 0) {
        fprintf(stderr, "Error creating directory %s: %s\n", dir_path, strerror(errno));
        return false;
    }
    return true;
}
