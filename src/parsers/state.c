/**
 * @file parser_state.c
 * @brief Implementation of parser state management
 *
 * This file provides the implementation for parser state management, which is responsible
 * for tracking the current state of parsing, handling multi-line content, and processing
 * various documentation tags. The parser state maintains the current file being processed,
 * configuration settings, and the current documentation block being built.
 */

#include "parsers/state.h"
#include "parsers/tag.h"
#include "parsers/function.h"
#include "parsers/metadata.h"
#include "parsers/annotation.h"
#include "parsers/example.h"
#include "parsers/argument.h"
#include "parsers/return.h"
#include "parsers/exitcode.h"
#include "parsers/alert.h"
#include "utils/string.h"
#include "utils/debug.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// External functions declared in other files
extern bool is_comment_line(const char *line);

/**
 * @brief Initialize the parser state
 * 
 * This function initializes the parser state structure and opens the file for reading.
 * It sets up the initial state for parsing a file, opening the specified file and
 * associating it with the parser state.
 *
 * @param state The parser state structure to initialize
 * @param file_path Path to the file to be parsed
 * @param config Configuration settings for the parser
 *
 * @return bool true if initialization was successful, false otherwise
 *              (e.g., if the file could not be opened or parameters are NULL)
 *
 * @note This function must be called before using any other parser state functions
 * @note The opened file will need to be closed using cleanup_parser_state()
 */
bool init_parser_state(parser_state_t *state, const char *file_path, const shellscribe_config_t *config) {
    if (state == NULL || file_path == NULL || config == NULL) {
        return false;
    }
    memset(state, 0, sizeof(parser_state_t));
    state->config = config;
    state->file = fopen(file_path, "r");
    if (state->file == NULL) {
        debug_message(config, "Failed to open file: %s\n", file_path);
        return false;
    }
    
    return true;
}

/**
 * @brief Clean up resources used by the parser state
 * 
 * This function closes the file associated with the parser state and frees any 
 * allocated memory. It should be called when parsing is complete or when an error 
 * occurs during parsing.
 *
 * @param state The parser state structure to clean up
 *
 * @note This function checks if the state or file pointer is NULL before attempting
 *       to close the file
 * @note This function does not free the state structure itself, only resources
 *       owned by it
 */
void cleanup_parser_state(parser_state_t *state) {
    if (state != NULL && state->file != NULL) {
        fclose(state->file);
        state->file = NULL;
    }
}

/**
 * @brief Collect content that continues across multiple lines
 * 
 * This function collects all lines of a continued comment block and returns the
 * concatenated content. It reads subsequent lines from the file until it encounters
 * a line that is not a continuation of the current comment or tag.
 *
 * @param state The current parser state
 * @param initial_content The initial content of the comment or tag
 *
 * @return char* A newly allocated string containing the concatenated content from
 *               all continuation lines, or NULL if an error occurred
 *
 * @note The returned string must be freed by the caller
 * @note This function preserves the file position if it encounters a line that is
 *       not part of the continued content
 * @note Continuation lines must be comment lines without tags or special annotations
 * @note This function adds newlines between the content from different lines
 */
char *state_collect_continued_content(parser_state_t *state, const char *initial_content) {
    if (state == NULL || initial_content == NULL || state->file == NULL) {
        return NULL;
    }
    long pos = ftell(state->file);
    char *accumulated_content = string_duplicate(initial_content);
    debug_message(state->config, "Collecting continued content starting with: '%s'\n", initial_content);
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), state->file) != NULL) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        if (is_comment_line(line) && !is_tag_line(line) && !is_special_annotation(line)) {
            const char *comment_start = strchr(line, '#');
            if (comment_start == NULL) {
                continue;
            }
            comment_start++;
            while (*comment_start && isspace(*comment_start)) {
                comment_start++;
            }
            debug_message(state->config, "Found continuation line: '%s'\n", comment_start);
            char *new_content = string_concat(accumulated_content, "\n");
            free(accumulated_content);
            char *temp = new_content;
            new_content = string_concat(temp, comment_start);
            free(temp);
            
            accumulated_content = new_content;
        } else {
            debug_message(state->config, "End of continuation detected: '%s'\n", line);
            fseek(state->file, pos, SEEK_SET);
            break;
        }
        pos = ftell(state->file);
    }
    
    debug_message(state->config, "Collected content: '%s'\n", accumulated_content);
    return accumulated_content;
}

/**
 * @brief Process a tag in the current parser state
 * 
 * This function processes a documentation tag and its content, updating the current
 * documentation block accordingly. It handles various tag types, including metadata
 * tags, function tags, parameter tags, return tags, and more.
 *
 * @param state The current parser state
 * @param tag The tag name (without the '@' prefix)
 * @param content The content associated with the tag
 *
 * @return bool true if the tag was successfully processed, false otherwise
 *              (e.g., if the tag is unknown or parameters are NULL)
 *
 * @note This function delegates the processing of specific tag types to specialized
 *       functions
 * @note For certain tags (e.g., description), this function collects content that
 *       continues across multiple lines
 * @note The current_block in the state must be initialized before calling this function
 */
bool state_process_tag(parser_state_t *state, const char *tag, const char *content) {
    if (state == NULL || tag == NULL || content == NULL || state->current_block == NULL) {
        return false;
    }
    debug_message(state->config, "Processing tag: @%s with content: %s\n", tag, content);
    if (is_file_level_tag(tag)) {
        return process_file_metadata_tag(state->current_block, tag, content);
    }
    if (strcmp(tag, "function") == 0) {
        free(state->current_block->function_name);
        char* parentheses = strstr(content, "()");
        if (parentheses != NULL && parentheses[2] == '\0') {
            char* cleaned_name = strndup(content, parentheses - content);
            state->current_block->function_name = cleaned_name;
        } else {
            state->current_block->function_name = string_duplicate(content);
        }
        state->in_docblock = true;
        return true;
    }
    if (strcmp(tag, "brief") == 0) {
        if (state->current_block->function_name == NULL) {
            free(state->current_block->brief);
            state->current_block->brief = string_duplicate(content);
        } else {
            free(state->current_block->function_brief);
            state->current_block->function_brief = string_duplicate(content);
        }
        return true;
    }
    if (strcmp(tag, "description") == 0) {
        char *accumulated_content = state_collect_continued_content(state, content);
        if (state->current_block->function_name == NULL) {
            free(state->current_block->description);
            state->current_block->description = accumulated_content;
        } else {
            free(state->current_block->function_description);
            state->current_block->function_description = accumulated_content;
        }
        return true;
    }
    if (strcmp(tag, "arg") == 0 || strcmp(tag, "argument") == 0) {
        return process_argument_tag(state->current_block, content);
    }
    if (strcmp(tag, "param") == 0) {
        return process_parameter_tag(state->current_block, content);
    }
    if (strcmp(tag, "return") == 0) {
        return process_return_tag(state->current_block, content);
    }
    if (strcmp(tag, "returns") == 0) {
        return process_returns_tag(state->current_block, content);
    }
    if (strcmp(tag, "exitcode") == 0) {
        return process_exitcode_tag(state->current_block, content);
    }
    if (strcmp(tag, "example") == 0) {
        return process_example_using_state(state, content);
    }
    if (strcmp(tag, "stdout") == 0) {
        char *accumulated_content = state_collect_continued_content(state, content);
        free(state->current_block->stdout_doc);
        state->current_block->stdout_doc = accumulated_content;
        return true;
    }
    if (strcmp(tag, "stderr") == 0) {
        free(state->current_block->stderr_doc);
        state->current_block->stderr_doc = string_duplicate(content);
        return true;
    }
    if (strcmp(tag, "internal") == 0) {
        state->current_block->is_internal = true;
        return true;
    }
    if (strcmp(tag, "note") == 0 || strcmp(tag, "warning") == 0 || 
        strcmp(tag, "error") == 0 || strcmp(tag, "tip") == 0 || 
        strcmp(tag, "important") == 0 || strcmp(tag, "info") == 0 || 
        strcmp(tag, "danger") == 0 || strcmp(tag, "hint") == 0 ||
        strcmp(tag, "caution") == 0 || strcmp(tag, "alert") == 0) {
        return process_alert_tag(state->current_block, tag, content);
    }
    debug_message(state->config, "Unknown tag: @%s\n", tag);
    return false;
}

/**
 * @brief Process an example tag using the current parser state
 * 
 * This function processes an example tag and adds it to the documentation block.
 * It extracts the example content from the tag and adds it to the current docblock.
 *
 * @param state The current parser state
 * @param content The content of the example tag
 *
 * @return bool true if the example was successfully processed and added to the
 *              docblock, false otherwise
 *
 * @note This function uses process_example_tag() to extract the example content
 *       and add_example_to_docblock() to add it to the documentation block
 * @note The example content is freed after being added to the docblock
 * @note This function handles memory allocation and deallocation for the example
 *       content
 */
bool process_example_using_state(parser_state_t *state, const char *content) {
    if (state == NULL || content == NULL || state->current_block == NULL) {
        return false;
    }
    char *example_content = process_example_tag(state, content);
    if (example_content == NULL) {
        return false;
    }
    bool result = add_example_to_docblock(state->current_block, example_content);
    free(example_content);
    
    return result;
} 
