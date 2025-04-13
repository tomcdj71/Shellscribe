/**
 * @file main.c
 * @brief Main entry point for shellscribe
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>  // For PATH_MAX

#include "core/shellscribe.h"
#include "utils/config.h"
#include "utils/debug.h"
#include "utils/memory.h"
#include "parsers/types.h"
#include "renderers/renderer_engine.h"

#define MAX_FILES 1000

// Define SHELLSCRIBE_VERSION if not defined already
#ifndef SHELLSCRIBE_VERSION
#define SHELLSCRIBE_VERSION "1.0.0"
#endif

// Define PATH_MAX if not defined
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_ORANGE  "\033[38;5;208m"

#define STATUS_OK        "[" COLOR_GREEN "OK" COLOR_RESET "]"
#define STATUS_SKIPPED   "[" COLOR_BLUE "SKIPPED" COLOR_RESET "]"
#define STATUS_FAILED    "[" COLOR_RED "FAILED" COLOR_RESET "]"
#define SKIP_TAG         COLOR_ORANGE "@skip" COLOR_RESET

// Forward declarations
static void print_version(void);
static void print_usage(const char *program_name);
static char *normalize_path(const char *path);
static int get_shell_scripts_recursive(const char *dir_path, char **files, int max_files, int current_count, bool traverse_symlinks);
static int get_shell_scripts(const char *dir_path, char **files, int max_files, const shellscribe_config_t *config);
static bool is_directory(const char *path);
static bool is_elf_binary(const char *file_path);
static bool should_skip_file(const char *input_file, const shellscribe_config_t *config, char **skip_reason);
static bool create_directories_recursive(const char *path);
static bool parse_arguments(int argc, char *argv[], char **input_file, char **config_file, bool *show_version, bool *show_help);
static bool initialize_config(shellscribe_config_t *config, const char *config_file);
static void finalize_config(shellscribe_config_t *config);
static int process_directory(const char *input_file, const shellscribe_config_t *config);
static int process_files(char **files, int file_count, const char *base_dir, const shellscribe_config_t *config);
static bool process_single_file(const char *file_path, char *display_path, const shellscribe_config_t *config, int *processed_files, int *skipped_files, int *failed_files);
static void handle_skipped_file(char *skip_reason, char *display_path, int *skipped_files);
static bool generate_documentation(const char *file_path, char *display_path, const shellscribe_config_t *config);
static bool prepare_output_path(const char *relative_path, const shellscribe_config_t *config, char *output_path);
static int process_file(const char *input_file, const shellscribe_config_t *config);

/**
 * Prints the program version
 * 
 * @return void
 */
static void print_version(void) {
    printf("%s\n", SHELLSCRIBE_VERSION);
}

/**
 * @brief Prints usage help
 * 
 * This function prints usage help for the shellscribe tool.
 * 
 * @param program_name The name of the program
 * @return void
 */
static void print_usage(const char *program_name) {
    printf("Usage: %s [options]|<input_file_or_directory>\n", program_name);
    printf("\n");
    printf("Options:\n");
    printf("  --help, -h         Display this help message\n");
    printf("  --version, -v      Display version information\n");
    printf("  --config-file=FILE, -c=FILE Specify a custom configuration file\n");
    printf("\n");
}

/**
 * @brief Normalize a path by removing double slashes
 * 
 * This function normalizes a path by removing double slashes.
 * 
 * @param path The path to normalize
 * @return char* The normalized path, or NULL on error
 */
static char *normalize_path(const char *path) {
    if (path == NULL) {
        return NULL;
    }
    char *result = shell_strdup(path);
    if (result == NULL) {
        return NULL;
    }
    char *src = result;
    char *dst = result;
    bool prev_was_slash = false;
    while (*src) {
        if (*src == '/' && prev_was_slash) {
            src++;
        } else {
            prev_was_slash = (*src == '/');
            *dst++ = *src++;
        }
    }
    
    *dst = '\0';
    return result;
}

/**
 * @brief Get all shell scripts in a directory, with optional recursion into subdirectories
 * 
 * Get all shell scripts in a directory, with optional recursion into subdirectories
 * 
 * @param dir_path The directory path to search
 * @param files The array to store the shell script paths
 * @param max_files The maximum number of files to store
 * @param current_count The current count of files
 * @param traverse_symlinks Whether to traverse symlinks
 * @return int The number of shell scripts found
 */
static int get_shell_scripts_recursive(const char *dir_path, char **files, int max_files, int current_count, bool traverse_symlinks) {
    if (files == NULL || max_files <= 0 || current_count >= max_files) {
        return current_count;
    }
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        return current_count;
    }
    int count = current_count;
    struct dirent *entry;
    char full_path[PATH_MAX];
    struct stat file_stat;
    while ((entry = readdir(dir)) != NULL && count < max_files) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        if (stat(full_path, &file_stat) != 0) {
            continue;
        }
        if (S_ISDIR(file_stat.st_mode)) {
            count = get_shell_scripts_recursive(full_path, files, max_files, count, traverse_symlinks);
        } else if (S_ISREG(file_stat.st_mode) || (S_ISLNK(file_stat.st_mode) && traverse_symlinks)) {
            const char *extensions[] = {".sh", ".bash", ".zsh"};
            for (size_t i = 0; i < sizeof(extensions) / sizeof(extensions[0]); i++) {
                if (strstr(entry->d_name, extensions[i])) {
                    files[count] = shell_strdup(full_path);
                    if (files[count] == NULL) {
                        fprintf(stderr, "Error: Failed to allocate memory for file path\n");
                        break;
                    }
                    count++;
                    break;
                }
            }
        }
    }

    closedir(dir);
    return count;
}

/**
 * @brief Get all shell scripts in a directory
 * 
 * This function gets all shell scripts in a directory.
 * 
 * @param dir_path The directory path to search
 * @param files The array to store the shell script paths
 * @param max_files The maximum number of files to store
 * @param config The configuration
 */
static int get_shell_scripts(const char *dir_path, char **files, int max_files, const shellscribe_config_t *config) {
    return get_shell_scripts_recursive(dir_path, files, max_files, 0, config->traverse_symlinks);
}

/**
 * @brief Check if path is a directory
 * 
 * This function checks if a path is a directory.
 * 
 * @param path The path to check
 * @return bool True if the path is a directory, false otherwise
 */
static bool is_directory(const char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        return false;
    }
    return S_ISDIR(path_stat.st_mode);
}

/**
 * Check if a file is an ELF binary
 */
static bool is_elf_binary(const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        return false;
    }
    unsigned char magic[4];
    size_t bytes_read = fread(magic, 1, 4, file);
    fclose(file);
    
    if (bytes_read != 4) {
        return false;
    }
    return (magic[0] == 0x7F && magic[1] == 'E' && magic[2] == 'L' && magic[3] == 'F');
}

/**
 * @brief Check if a file should be skipped based on the @skip annotation or if it's an ELF binary
 * 
 * This function checks if a file should be skipped based on the @skip annotation or if it's an ELF binary.
 * 
 * @param input_file The input file to check
 * @param config The configuration
 * @param skip_reason The skip reason
 */
static bool should_skip_file(const char *input_file, const shellscribe_config_t *config, char **skip_reason) {
    if (is_elf_binary(input_file)) {
        if (skip_reason) {
            *skip_reason = shell_strdup("ELF binary detected");
        }
        return true;
    }
    int block_count = 0;
    shellscribe_docblock_t *docblocks = parse_shell_script(input_file, &block_count, config);
    if (docblocks == NULL || block_count <= 0) {
        return false;
    }
    bool should_skip = (block_count > 0 && docblocks[0].is_skipped);
    if (should_skip && skip_reason) {
        *skip_reason = shell_strdup("marked with @skip");
    }
    free_docblocks(docblocks, block_count);
    return should_skip;
}

/**
 * Create a directory and all parent directories recursively
 * 
 * @param path Directory path to create
 * @return true if successful, false otherwise
 */
static bool create_directories_recursive(const char *path) {
    if (path == NULL) {
        return false;
    }
    char tmp[PATH_MAX];
    char *p = NULL;
    size_t len;
    strncpy(tmp, path, PATH_MAX - 1);
    tmp[PATH_MAX - 1] = '\0';
    len = strlen(tmp);
    if (len > 0 && tmp[len - 1] == '/') {
        tmp[len - 1] = '\0';
    }
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            struct stat st;
            if (stat(tmp, &st) != 0) {
                if (mkdir(tmp, 0755) != 0) {
                    return false;
                }
            } else if (!S_ISDIR(st.st_mode)) {
                return false;
            }
            *p = '/';
        }
    }
    struct stat st;
    if (stat(tmp, &st) != 0) {
        if (mkdir(tmp, 0755) != 0) {
            return false;
        }
    } else if (!S_ISDIR(st.st_mode)) {
        return false;
    }

    return true;
}

/**
 * Extract relative path from a full path based on a base directory
 * 
 * @param full_path Full path to process
 * @param base_dir Base directory to extract relative path from
 * @return Newly allocated string with relative path or NULL if error
 */
static char *get_relative_path(const char *full_path, const char *base_dir) {
    if (!full_path || !base_dir) {
        return NULL;
    }
    char *norm_full_path = normalize_path(full_path);
    char *norm_base_dir = normalize_path(base_dir);
    if (!norm_full_path || !norm_base_dir) {
        shell_free((void **)&norm_full_path);
        shell_free((void **)&norm_base_dir);
        return NULL;
    }
    size_t base_len = strlen(norm_base_dir);
    if (base_len > 0 && norm_base_dir[base_len - 1] != '/') {
        char *new_base = shell_malloc(base_len + 2);
        if (!new_base) {
            shell_free((void **)&norm_full_path);
            shell_free((void **)&norm_base_dir);
            return NULL;
        }
        snprintf(new_base, base_len + 2, "%s/", norm_base_dir);
        shell_free((void **)&norm_base_dir);
        norm_base_dir = new_base;
        base_len++;
    }
    char *relative_path = NULL;
    if (strncmp(norm_full_path, norm_base_dir, base_len) == 0) {
        relative_path = shell_strdup(norm_full_path + base_len);
    } else {
        const char *filename = strrchr(norm_full_path, '/');
        relative_path = shell_strdup(filename ? filename + 1 : norm_full_path);
    }
    shell_free((void **)&norm_full_path);
    shell_free((void **)&norm_base_dir);
    return relative_path;
}

/**
 * @brief Main function
 * 
 * This function is the main entry point for the shellscribe tool.
 * 
 * @param argc The number of command-line arguments
 * @param argv The command-line arguments
 */
int main(int argc, char *argv[]) {
    char *input_file = NULL;
    char *config_file = NULL;
    bool show_version = false;
    bool show_help = false;
    if (!parse_arguments(argc, argv, &input_file, &config_file, &show_version, &show_help)) {
        return 1;
    }
    if (show_version) {
        print_version();
        return 0;
    }
    if (show_help || input_file == NULL) {
        print_usage(argv[0]);
        return 0;
    }
    shellscribe_config_t config;
    if (!initialize_config(&config, config_file)) {
        return 1;
    }
    bool is_dir = is_directory(input_file);
    int result = 0;
    result = is_dir ? process_directory(input_file, &config) : process_file(input_file, &config);
    finalize_config(&config);

    return result;
}

/**
 * @brief Parse command-line arguments
 * 
 * This function parses command-line arguments and sets the appropriate variables.
 * 
 * @param argc The number of command-line arguments
 * @param argv The command-line arguments
 * @param input_file Pointer to store the input file or directory
 * @param config_file Pointer to store the configuration file
 * @param show_version Pointer to store whether to show version information
 * @param show_help Pointer to store whether to show help information
 * @return bool True if parsing was successful, false otherwise
 */
static bool parse_arguments(int argc, char *argv[], char **input_file, char **config_file, bool *show_version, bool *show_help) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            *show_help = true;
        } else if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            *show_version = true;
        } else if (strncmp(argv[i], "--config-file=", 14) == 0 || strncmp(argv[i], "-c=", 3) == 0) {
            *config_file = argv[i] + 14;
        } else if (argv[i][0] != '-') {
            *input_file = argv[i];
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            return false;
        }
    }
    return true;
}

/**
 * @brief Initialize the configuration
 * 
 * This function initializes the configuration by loading it from a file and setting up memory tracking.
 * 
 * @param config Pointer to the configuration structure
 * @param config_file Path to the configuration file
 * @return bool True if initialization was successful, false otherwise
 */
static bool initialize_config(shellscribe_config_t *config, const char *config_file) {
    if (!load_config(config, config_file)) {
        fprintf(stderr, "Error: unable to load configuration\n");
        return false;
    }
    char *debug_env = getenv("SHELLSCRIBE_DEBUG");
    if (debug_env != NULL && strcmp(debug_env, "1") == 0) {
        config->memory_tracking = true;
        fprintf(stderr, "Debug env detected: Memory tracking enabled\n");
    }
    if (config->memory_tracking) {
        shell_memory_tracking_enable();
        fprintf(stderr, "Memory tracking enabled\n");
    }

    return true;
}

/**
 * @brief Finalize the configuration
 * 
 * This function finalizes the configuration by cleaning up memory tracking and printing memory statistics.
 * 
 * @param config Pointer to the configuration structure
 */
static void finalize_config(shellscribe_config_t *config) {
    if (config->memory_tracking && config->memory_stats) {
        shell_memory_stats();
        if (shell_check_leaks()) {
            fprintf(stderr, "Warning: Memory leaks detected\n");
        }
    }
    if (config->memory_tracking) {
        shell_memory_cleanup();
    }
}

/**
 * @brief Process a single file
 * 
 * This function processes a single file and generates documentation.
 * 
 * @param input_file The input file to process
 * @param config The configuration
 * @return int 0 on success, 1 on failure
 */
static int process_directory(const char *input_file, const shellscribe_config_t *config) {
    fprintf(stderr, "Processing shell scripts in directory: %s\n", input_file);
    char *files[MAX_FILES];
    int file_count = get_shell_scripts(input_file, files, MAX_FILES, config);
    if (file_count <= 0) {
        fprintf(stderr, "Error: No shell scripts found in directory\n");
        return 1;
    }
    fprintf(stderr, "Found %d shell scripts to process\n", file_count);
    int result = process_files(files, file_count, input_file, config);
    for (int i = 0; i < file_count; i++) {
        if (files[i] != NULL) {
            void *ptr = files[i];
            shell_free(&ptr);
        }
    }

    return result;
}

/**
 * @brief Process a single file
 * 
 * This function processes a single file and generates documentation.
 * 
 * @param input_file The input file to process
 * @param config The configuration
 * @return int 0 on success, 1 on failure
 */
static int process_files(char **files, int file_count, const char *base_dir, const shellscribe_config_t *config) {
    int failed_files = 0;
    int processed_files = 0;
    int skipped_files = 0;
    for (int i = 0; i < file_count; i++) {
        const char *file_path = files[i];
        char *display_path = get_relative_path(file_path, base_dir);
        if (!process_single_file(file_path, display_path, config, &processed_files, &skipped_files, &failed_files)) {
            continue;
        }
    }
    fprintf(stderr, "\nSummary: %d OK, %d SKIPPED, %d FAILED (total: %d)\n", processed_files, skipped_files, failed_files, file_count);

    return (failed_files > 0) ? 1 : 0;
}

/**
 * @brief Process a single file
 * 
 * This function processes a single file and generates documentation.
 * 
 * @param file_path The input file to process
 * @param display_path The display path for the file
 * @param config The configuration
 * @param processed_files Pointer to store the number of processed files
 * @param skipped_files Pointer to store the number of skipped files
 * @param failed_files Pointer to store the number of failed files
 * @return bool True if processing was successful, false otherwise
 */
static bool process_single_file(const char *file_path, char *display_path, const shellscribe_config_t *config, int *processed_files, int *skipped_files, int *failed_files) {
    if (display_path == NULL) {
        const char *base_name = strrchr(file_path, '/');
        base_name = base_name ? base_name + 1 : file_path;
        fprintf(stderr, "%-40s ", base_name);
    } else {
        fprintf(stderr, "%-40s ", display_path);
    }
    char *skip_reason = NULL;
    if (should_skip_file(file_path, config, &skip_reason)) {
        handle_skipped_file(skip_reason, display_path, skipped_files);
        return false;
    }
    if (!generate_documentation(file_path, display_path, config)) {
        (*failed_files)++;
        return false;
    }
    fprintf(stderr, STATUS_OK "\n");
    (*processed_files)++;
    return true;
}

/**
 * @brief Handle skipped file
 * 
 * This function handles a skipped file by printing the reason and updating the count.
 * 
 * @param skip_reason The reason for skipping the file
 * @param display_path The display path for the file
 * @param skipped_files Pointer to store the number of skipped files
 */
static void handle_skipped_file(char *skip_reason, char *display_path, int *skipped_files) {
    if (strstr(skip_reason, "marked with @skip") != NULL) {
        fprintf(stderr, STATUS_SKIPPED " (marked with %s)\n", SKIP_TAG);
    } else {
        fprintf(stderr, STATUS_SKIPPED " (%s)\n", skip_reason);
    }
    if (skip_reason) {
        void *reason_ptr = skip_reason;
        shell_free(&reason_ptr);
    }
    if (display_path) {
        void *ptr = display_path;
        shell_free(&ptr);
    }
    (*skipped_files)++;
}

/**
 * @brief Generate documentation for a file
 * 
 * This function generates documentation for a file and writes it to the output path.
 * 
 * @param file_path The input file to process
 * @param display_path The display path for the file
 * @param config The configuration
 * @return bool True if generation was successful, false otherwise
 */
static bool generate_documentation(const char *file_path, char *display_path, const shellscribe_config_t *config) {
    char *relative_path = display_path ? shell_strdup(display_path) : get_relative_path(file_path, config->filename);
    if (relative_path == NULL) {
        fprintf(stderr, STATUS_FAILED " (error determining relative path)\n");
        return false;
    }
    char output_path[PATH_MAX];
    if (!prepare_output_path(relative_path, config, output_path)) {
        shell_free((void **)&relative_path);
        return false;
    }
    shell_free((void **)&relative_path);
    FILE *output = fopen(output_path, "w");
    if (output == NULL) {
        fprintf(stderr, STATUS_FAILED " (error opening output file)\n");
        return false;
    }
    int block_count = 0;
    shellscribe_docblock_t *docblocks = parse_shell_script(file_path, &block_count, config);
    if (docblocks == NULL || block_count <= 0) {
        fprintf(stderr, STATUS_FAILED " (error parsing documentation)\n");
        fclose(output);
        return false;
    }
    bool success = render_documentation((const shellscribe_docblock_t *)docblocks, block_count, output, config);
    free_docblocks(docblocks, block_count);
    fclose(output);
    if (!success) {
        fprintf(stderr, STATUS_FAILED " (error generating documentation)\n");
        return false;
    }

    return true;
}

/**
 * @brief Prepare the output path for the generated documentation
 * 
 * This function prepares the output path for the generated documentation.
 * 
 * @param relative_path The relative path to process
 * @param config The configuration
 * @param output_path The output path to store the result
 * @return bool True if preparation was successful, false otherwise
 */
static bool prepare_output_path(const char *relative_path, const shellscribe_config_t *config, char *output_path) {
    char dir_path[PATH_MAX] = {0};
    char *last_slash = strrchr(relative_path, '/');
    if (last_slash) {
        strncpy(dir_path, relative_path, last_slash - relative_path);
        dir_path[last_slash - relative_path] = '\0';

        char doc_path_path[PATH_MAX];
        snprintf(doc_path_path, sizeof(doc_path_path), "%s/%s", config->doc_path, dir_path);
        if (!create_directories_recursive(doc_path_path)) {
            fprintf(stderr, STATUS_FAILED " (error creating output directory)\n");
            return false;
        }
    }
    const char *output_base_name = last_slash ? last_slash + 1 : relative_path;
    char *ext = strrchr(output_base_name, '.');
    if (last_slash) {
        if (ext) {
            int name_len = ext - output_base_name;
            snprintf(output_path, PATH_MAX, "%s/%s/%.*s.md", config->doc_path, dir_path, name_len, output_base_name);
        } else {
            snprintf(output_path, PATH_MAX, "%s/%s/%s.md", config->doc_path, dir_path, output_base_name);
        }
    } else {
        if (ext) {
            int name_len = ext - output_base_name;
            snprintf(output_path, PATH_MAX, "%s/%.*s.md", config->doc_path, name_len, output_base_name);
        } else {
            snprintf(output_path, PATH_MAX, "%s/%s.md", config->doc_path, output_base_name);
        }
    }

    return true;
}

/**
 * @brief Process a single file
 * 
 * This function processes a single file and generates documentation.
 * 
 * @param input_file The input file to process
 * @param config The configuration
 * @return int 0 on success, 1 on failure
 */
static int process_file(const char *input_file, const shellscribe_config_t *config) {
    const char *base_name = strrchr(input_file, '/');
    base_name = base_name ? base_name + 1 : input_file;
    fprintf(stderr, "%-40s ", base_name);
    char *skip_reason = NULL;
    if (should_skip_file(input_file, config, &skip_reason)) {
        handle_skipped_file(skip_reason, NULL, NULL);
        return 0;
    }
    if (!generate_documentation(input_file, NULL, config)) {
        return 1;
    }

    return 0;
}
