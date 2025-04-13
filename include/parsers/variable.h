/**
 * @file variable.h
 * @brief Parser for @set tag in shell script documentation
 */

#ifndef SHELLSCRIBE_VARIABLE_H
#define SHELLSCRIBE_VARIABLE_H

#include "parsers/types.h"
#include <stdbool.h>

/**
 * @brief Process a set tag
 *
 * @param docblock The documentation block
 * @param content The tag content
 * @return true if successful, false otherwise
 */
bool process_set_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Check if a tag is a set tag
 *
 * @param tag The tag name
 * @return true if the tag is a set tag, false otherwise
 */
bool is_set_tag(const char *tag);

/**
 * @brief Parse the set content to extract the variable name, type and description
 * 
 * @param content The set content to parse
 * @param name Output parameter for the extracted variable name
 * @param type Output parameter for the extracted variable type
 * @param description Output parameter for the extracted description
 * @return true if parsing was successful, false otherwise
 */
bool parse_set_content(const char *content, char **name, char **type, char **description);

#endif /* SHELLSCRIBE_VARIABLE_H */ 
