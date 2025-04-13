/**
 * @file exitcode.h
 * @brief Parser for @exitcode tag in shell script documentation
 */

#ifndef SHELLSCRIBE_EXITCODE_H
#define SHELLSCRIBE_EXITCODE_H

#include "parsers/types.h"
#include <stdbool.h>

/**
 * @brief Process an exitcode tag
 *
 * @param docblock The documentation block
 * @param content The tag content
 * @return true if successful, false otherwise
 */
bool process_exitcode_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Check if a tag is an exitcode tag
 *
 * @param tag The tag name
 * @return true if the tag is an exitcode tag, false otherwise
 */
bool is_exitcode_tag(const char *tag);

/**
 * @brief Parse the exitcode content to extract the code and its description
 * 
 * @param content The exitcode content to parse
 * @param code Output parameter for the extracted exit code
 * @param description Output parameter for the extracted description
 * @return true if parsing was successful, false otherwise
 */
bool parse_exitcode_content(const char *content, char **code, char **description);

#endif /* SHELLSCRIBE_EXITCODE_H */ 
