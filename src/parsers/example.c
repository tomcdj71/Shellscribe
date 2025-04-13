/**
 * @file example.c
 * @brief Implementation of the example parser
 *
 * This file provides functionality for parsing and processing example tags
 * in documentation blocks. Examples are crucial for providing concrete usage
 * demonstrations of functions, making the documentation more practical and usable.
 * The parser extracts example content from comments and can handle multi-line
 * examples that span across consecutive comment lines.
 */

#include "parsers/example.h"
#include "parsers/tag.h"
#include "parsers/annotation.h"
#include "parsers/state.h"
#include "utils/string.h"
#include "utils/debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// External function declarations
extern bool is_comment_line(const char *line);
extern bool is_tag_line(const char *line);
extern bool is_special_annotation(const char *line);

#define MAX_LINE_LENGTH 4096

/**
 * @brief Check if a tag is an example tag
 * 
 * Determines whether a given tag name is an example tag.
 * Example tags provide code snippets or usage demonstrations for functions.
 *
 * @param tag The tag name to check, without the '@' prefix
 * 
 * @return bool true if the tag is an example tag, false otherwise or if tag is NULL
 *
 * @note This function is case-sensitive, so the tag must exactly match "example"
 */
bool is_example_tag(const char *tag) {
    if (tag == NULL) {
        return false;
    }
    
    return (strcmp(tag, "example") == 0);
}

/**
 * @brief Extracts example content from documentation comments
 * 
 * Reads subsequent lines from a file and collects example content from comment
 * lines that follow the initial example tag. This function handles multi-line
 * examples that are spread across consecutive comment lines.
 *
 * The function stops collecting when it encounters a line that is not a comment,
 * is a tag line, or contains a special annotation.
 *
 * @param file File handle to read subsequent lines from
 * @param initial_content The content from the first line of the example
 * @param config Configuration settings for parsing
 * 
 * @return char* Newly allocated string containing the complete example content,
 *               or NULL on error. The caller is responsible for freeing this memory.
 *
 * @note Lines are separated by newlines in the returned string
 * @note The function logs debug messages if enabled in the configuration
 * @note Example content collection stops at the first non-comment line, tag line,
 *       or special annotation line encountered
 * @note Memory is allocated for the example content, which must be freed by the caller
 */
char *extract_example_content(FILE *file, const char *initial_content, const shellscribe_config_t *config) {
    if (file == NULL || initial_content == NULL || config == NULL) {
        return NULL;
    }
    long pos = ftell(file);
    char *example_content = string_duplicate(initial_content);
    if (example_content == NULL) {
        return NULL;
    }
    
    debug_message(config, "Extracting example starting with: '%s'\n", initial_content);
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }
        if (is_comment_line(line) && !is_tag_line(line) && !is_special_annotation(line)) {
            const char *comment_start = strchr(line, '#');
            if (comment_start == NULL) continue;
            comment_start++;
            char *new_content = string_concat(example_content, "\n");
            free(example_content);
            char *temp = new_content;
            new_content = string_concat(temp, comment_start);
            free(temp);
            
            example_content = new_content;
            
            debug_message(config, "  Added example line: '%s'\n", comment_start);
        } else {
            debug_message(config, "  End of example at line: '%s'\n", line);
            fseek(file, pos, SEEK_SET);
            break;
        }

        pos = ftell(file);
    }
    
    debug_message(config, "Extracted example content: '%s'\n", example_content);
    return example_content;
}

/**
 * @brief Adds an example to a documentation block
 * 
 * Adds or appends example content to a documentation block. If the docblock
 * already has example content, the new example is appended after two newlines
 * to separate examples clearly.
 *
 * @param docblock The documentation block to add the example to
 * @param example_content The example content to add
 * 
 * @return bool true if the example was successfully added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 *
 * @note If the docblock already has example content, the new content is appended
 *       with two newlines as a separator
 * @note Memory is allocated for the example content, which will be freed
 *       when the docblock is freed
 * @note Multiple examples in a single docblock will be rendered based on the
 *       configured example display style (sequential or tabbed)
 */
bool add_example_to_docblock(shellscribe_docblock_t *docblock, const char *example_content) {
    if (docblock == NULL || example_content == NULL) {
        return false;
    }
    if (docblock->example != NULL) {
        char *new_example = string_concat(docblock->example, "\n\n");
        free(docblock->example);
        
        char *temp = new_example;
        new_example = string_concat(temp, example_content);
        free(temp);
        
        docblock->example = new_example;
    } else {
        docblock->example = string_duplicate(example_content);
    }
    
    return true;
}

/**
 * @brief Process an example tag within a parser state
 * 
 * Processes an example tag and collects all the example content from subsequent
 * comment lines. This function uses the parser state to track the current position
 * in the file and collects content until a non-comment line, tag line, or special
 * annotation is encountered.
 *
 * @param state The current parser state, containing file handle and line buffer
 * @param content The initial content from the example tag line
 * 
 * @return char* Newly allocated string containing the complete example content,
 *               or NULL on error. The caller is responsible for freeing this memory.
 *
 * @note The function restores the file position if it reaches the end of the example
 * @note This function is similar to extract_example_content but uses the parser state
 *       for tracking position
 * @note Memory is allocated for the example content, which must be freed by the caller
 * 
 * @see extract_example_content
 */
char *process_example_tag(parser_state_t *state, const char *content) {
    if (state == NULL || content == NULL) {
        return NULL;
    }
    long pos = ftell(state->file);
    char line[MAX_LINE_LENGTH];
    char *example_content = string_duplicate(content);
    
    while (fgets(line, sizeof(line), state->file) != NULL) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }
        
        if (is_comment_line(line) && !is_tag_line(line) && !is_special_annotation(line)) {
            const char *comment_start = strchr(line, '#');
            if (comment_start == NULL) continue;
            comment_start++;
            char *new_content = string_concat(example_content, "\n");
            free(example_content);
            char *temp = new_content;
            new_content = string_concat(temp, comment_start);
            free(temp);
            
            example_content = new_content;
        } else {
            fseek(state->file, pos, SEEK_SET);
            break;
        }
        
        pos = ftell(state->file);
    }
    
    return example_content;
} 
