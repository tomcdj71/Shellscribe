/**
 * @file state.h
 * @brief Parser state management
 */

#ifndef SHELLSCRIBE_STATE_H
#define SHELLSCRIBE_STATE_H

#include "parsers/types.h"
#include "utils/config.h"
#include <stdio.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 4096

/**
 * @brief Parser state structure
 */
typedef struct parser_state {
    FILE *file;                    // Current file being parsed
    shellscribe_docblock_t *current_block; // Current documentation block
    char line[MAX_LINE_LENGTH];    // Current line buffer
    int line_number;               // Current line number
    bool in_docblock;              // Whether we're inside a documentation block
    const char *file_path;         // Path to the file being parsed
    const shellscribe_config_t *config; // Configuration options
} parser_state_t;

/**
 * @brief Initialize the parser state
 * 
 * @param state Parser state to initialize
 * @param file_path Path to the file to parse
 * @param config Configuration options
 * @return true if successful, false otherwise
 */
bool init_parser_state(parser_state_t *state, const char *file_path, const shellscribe_config_t *config);

/**
 * @brief Clean up the parser state
 * 
 * @param state Parser state to clean up
 */
void cleanup_parser_state(parser_state_t *state);

/**
 * @brief Process a tag in the current parser state
 * 
 * @param state Current parser state
 * @param tag Tag name
 * @param content Tag content
 * @return true if successful, false otherwise
 */
bool state_process_tag(parser_state_t *state, const char *tag, const char *content);

/**
 * @brief Process an example tag using the current parser state
 * 
 * @param state Current parser state
 * @param content Initial content of the example
 * @return true if successful, false otherwise
 */
bool process_example_using_state(parser_state_t *state, const char *content);

#endif /* SHELLSCRIBE_STATE_H */ 
