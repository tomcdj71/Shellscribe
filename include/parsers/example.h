/**
 * @file example.h
 * @brief Parser for examples in shell script documentation
 */

#ifndef SHELLSCRIBE_EXAMPLE_H
#define SHELLSCRIBE_EXAMPLE_H

#include "parsers/types.h"
#include "utils/config.h"
#include <stdio.h>
#include <stdbool.h>

// Forward declaration
struct parser_state;
typedef struct parser_state parser_state_t;

/**
 * @brief Extracts example content from documentation comments
 *
 * @param file File pointer at the current position
 * @param initial_content Initial content of the example
 * @param config Configuration options
 * @return Extracted example content
 */
char *extract_example_content(FILE *file, const char *initial_content, const shellscribe_config_t *config);

/**
 * @brief Adds an example to a documentation block
 *
 * @param docblock The documentation block
 * @param example_content The example content to add
 * @return true if successful, false otherwise
 */
bool add_example_to_docblock(shellscribe_docblock_t *docblock, const char *example_content);

/**
 * @brief Process an example tag in a documentation block
 * 
 * @param state Parser state containing file and docblock
 * @param content Initial content of the example
 * @return Processed example content, or NULL on error
 */
char *process_example_tag(parser_state_t *state, const char *content);

/**
 * @brief Check if a tag is an example tag
 * 
 * @param tag The tag name
 * @return true if the tag is an example tag, false otherwise
 */
bool is_example_tag(const char *tag);

#endif /* SHELLSCRIBE_EXAMPLE_H */ 
