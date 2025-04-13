/**
 * @file tag.c
 * @brief Implementation of tag parser for extracting documentation tags
 *
 * This file contains functions for identifying, extracting, and processing
 * documentation tags from shell script comments. Tags are special markers that
 * indicate the purpose of documentation sections (e.g., @param, @return, @example).
 * The tag parser is responsible for identifying these markers and extracting their
 * associated content for further processing.
 */

#include "parsers/tag.h"
#include "utils/string.h"
#include "utils/debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Constants
#define TAG_PREFIX "# @"

/**
 * @brief Check if a line is a tag line
 * 
 * Determines whether a given line contains a documentation tag by checking
 * for the tag prefix (# @) after any leading whitespace.
 *
 * @param line The line to check for a tag
 * 
 * @return bool true if the line contains a tag, false otherwise or if line is NULL
 */
static bool static_is_tag_line(const char *line) {
    if (line == NULL) {
        return false;
    }
    while (isspace(*line)) {
        line++;
    }
    return (strncmp(line, TAG_PREFIX, strlen(TAG_PREFIX)) == 0);
}

/**
 * @brief Extract the tag name from a line
 * 
 * Parses a line containing a documentation tag and extracts the tag name
 * (the text immediately following the # @ prefix, up to the first whitespace).
 * 
 * @param line The line containing the tag
 * 
 * @return char* Newly allocated string containing just the tag name,
 *               or NULL if the line does not contain a tag or if memory allocation fails.
 *               The caller is responsible for freeing this memory.
 */
static char *static_extract_tag_name(const char *line) {
    if (!static_is_tag_line(line)) {
        return NULL;
    }
    const char *tag_start = line + strlen(TAG_PREFIX);
    const char *tag_end = tag_start;
    while (*tag_end && !isspace(*tag_end)) {
        tag_end++;
    }
    size_t tag_length = tag_end - tag_start;
    char *tag_name = malloc(tag_length + 1);
    if (tag_name == NULL) {
        return NULL;
    }
    strncpy(tag_name, tag_start, tag_length);
    tag_name[tag_length] = '\0';
    
    return tag_name;
}

/**
 * @brief Extract the content associated with a tag
 * 
 * Parses a line containing a documentation tag and extracts the content
 * (everything after the tag name and any whitespace).
 * 
 * @param line The line containing the tag and its content
 * 
 * @return char* Newly allocated string containing just the tag content,
 *               or NULL if the line does not contain a tag or if memory allocation fails.
 *               The caller is responsible for freeing this memory.
 */
static char *static_extract_tag_content(const char *line) {
    if (!static_is_tag_line(line)) {
        return NULL;
    }
    const char *content_start = line + strlen(TAG_PREFIX);
    while (*content_start && !isspace(*content_start)) {
        content_start++;
    }
    while (isspace(*content_start)) {
        content_start++;
    }

    return string_duplicate(content_start);
}

/**
 * @brief Collect content from continued comment lines
 * 
 * Reads subsequent lines from a file and collects content from comment lines
 * that form a continued block of documentation. This is used to handle multi-line
 * documentation comments that are spread across several lines.
 * 
 * @param file File handle to read subsequent lines from
 * @param buffer Buffer to store each line during processing
 * @param buffer_size Size of the buffer
 * @param initial_content The content from the first line of the comment
 * 
 * @return char* Newly allocated string containing the concatenated content
 *               of all comment lines in the block, or NULL on error.
 *               The caller is responsible for freeing this memory.
 * 
 * @note This function will restore the file position if it encounters a line
 *       that is not part of the continued comment block.
 * @note Lines are separated by newlines in the returned string.
 */
char *collect_continued_comment_content(FILE *file, char *buffer, size_t buffer_size, const char *initial_content) {
    if (file == NULL || buffer == NULL || buffer_size == 0 || initial_content == NULL) {
        return NULL;
    }
    char *accumulated_content = string_duplicate(initial_content);
    if (accumulated_content == NULL) {
        return NULL;
    }
    long file_pos = ftell(file);
    if (file_pos < 0) {
        free(accumulated_content);
        return NULL;
    }
    while (fgets(buffer, buffer_size, file) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }
        char *line = buffer;
        while (isspace(*line)) {
            line++;
        }
        if (strncmp(line, "#", 1) == 0 && strncmp(line, TAG_PREFIX, strlen(TAG_PREFIX)) != 0) {
            line++;
            while (isspace(*line)) {
                line++;
            }
            char *new_content = NULL;
            if (asprintf(&new_content, "%s\n%s", accumulated_content, line) != -1) {
                free(accumulated_content);
                accumulated_content = new_content;
            }
        } else {
            fseek(file, file_pos, SEEK_SET);
            break;
        }
        file_pos = ftell(file);
        if (file_pos < 0) {
            break;
        }
    }
    
    return accumulated_content;
}

/**
 * @brief Check if a line contains a special annotation
 * 
 * Determines whether a given line contains a special annotation,
 * specifically a shellcheck directive.
 * 
 * @param line The line to check for special annotations
 * 
 * @return bool true if the line contains a special annotation, false otherwise or if line is NULL
 */
static bool static_is_special_annotation(const char *line) {
    if (line == NULL) {
        return false;
    }
    while (isspace(*line)) {
        line++;
    }
    
    return (strncmp(line, "# shellcheck", 12) == 0);
}

/**
 * @brief Process a tag and its content in the current documentation block
 * 
 * This is a stub function that serves as a placeholder for tag processing.
 * Concrete implementations for specific tags are in their respective files.
 * 
 * @param docblock The documentation block to update with the tag information
 * @param tag The tag name (without the @ prefix)
 * @param content The content associated with the tag
 * @param config Configuration settings for parsing
 * 
 * @return bool true if the tag was successfully processed, false otherwise
 * 
 * @note This function returns true regardless of input to indicate that tag processing
 *       is handled by specialized functions elsewhere.
 */
bool process_tag(shellscribe_docblock_t *docblock, const char *tag, const char *content, const shellscribe_config_t *config) {
    if (docblock == NULL || tag == NULL || content == NULL || config == NULL) {
        return false;
    }
    
    return true;
}

/**
 * @brief Collect continued content from a file
 * 
 * This is a wrapper around collect_continued_comment_content that provides a
 * simpler interface with a predefined buffer.
 * 
 * @param file File handle to read subsequent lines from
 * @param initial_content The content from the first line of the comment
 * @param config Configuration settings for parsing
 * 
 * @return char* Newly allocated string containing the concatenated content
 *               of all comment lines in the block, or NULL on error.
 *               The caller is responsible for freeing this memory.
 * 
 * @see collect_continued_comment_content
 */
char *collect_continued_content(FILE *file, const char *initial_content, const shellscribe_config_t *config) {
    if (file == NULL || initial_content == NULL || config == NULL) {
        return NULL;
    }
    char buffer[1024];
    
    return collect_continued_comment_content(file, buffer, sizeof(buffer), initial_content);
}

/**
 * @brief Check if a line is a tag line
 * 
 * Public wrapper around static_is_tag_line that determines whether
 * a given line contains a documentation tag.
 * 
 * @param line The line to check for a tag
 * 
 * @return bool true if the line contains a tag, false otherwise or if line is NULL
 * 
 * @see static_is_tag_line
 */
bool is_tag_line(const char *line) {
    return static_is_tag_line(line);
}

/**
 * @brief Extract tag name from a line
 * 
 * Parses a line containing a documentation tag and extracts the tag name.
 * This implementation is more robust than static_extract_tag_name and
 * handles different formats of tag lines.
 * 
 * @param line The line containing the tag
 * 
 * @return char* Newly allocated string containing just the tag name,
 *               or NULL if the line does not contain a tag or if memory allocation fails.
 *               The caller is responsible for freeing this memory.
 * 
 * @note This function specifically looks for the @ symbol to identify tags
 * @note Tag names are extracted up to the first whitespace or colon
 */
char *extract_tag_name(const char *line) {
    if (line == NULL) {
        return NULL;
    }
    const char *tag_start = strstr(line, "@");
    if (tag_start == NULL) {
        return NULL;
    }
    tag_start++;
    const char *tag_end = tag_start;
    while (*tag_end && !isspace((unsigned char)*tag_end) && *tag_end != ':') {
        tag_end++;
    }
    size_t tag_len = tag_end - tag_start;
    char *tag_name = malloc(tag_len + 1);
    if (tag_name == NULL) {
        return NULL;
    }
    strncpy(tag_name, tag_start, tag_len);
    tag_name[tag_len] = '\0';
    
    return tag_name;
}

/**
 * @brief Extract tag content from a line
 * 
 * Parses a line containing a documentation tag and extracts the content
 * (everything after the tag name and any whitespace).
 * 
 * @param line The line containing the tag and its content
 * 
 * @return char* Newly allocated string containing just the tag content,
 *               or NULL if the line does not contain a tag or if memory allocation fails.
 */
char *extract_tag_content(const char *line) {
    if (line == NULL) {
        return NULL;
    }
    const char *tag_start = strstr(line, "@");
    if (tag_start == NULL) {
        return NULL;
    }
    const char *content_start = tag_start + 1;
    while (*content_start && !isspace((unsigned char)*content_start) && *content_start != ':') {
        content_start++;
    }
    if (*content_start == ':') {
        content_start++;
    }
    while (isspace((unsigned char)*content_start)) {
        content_start++;
    }
    return string_duplicate(content_start);
}

/**
 * @brief Check if a line is a comment
 * 
 * Determines whether a given line is a comment line by checking if it starts
 * with a hash (#) character after any leading whitespace.
 *
 * @param line The line to check for a comment
 * 
 * @return bool true if the line is a comment, false otherwise or if line is NULL
 */
bool is_comment_line(const char *line) {
    if (line == NULL) {
        return false;
    }
    while (isspace((unsigned char)*line)) {
        line++;
    }
    
    return (line[0] == '#');
}
