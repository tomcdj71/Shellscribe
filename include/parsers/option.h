/**
 * @file option.h
 * @brief Parser for @option tag in shell script documentation
 */

#ifndef SHELLSCRIBE_OPTION_H
#define SHELLSCRIBE_OPTION_H

#include "parsers/types.h"
#include <stdbool.h>

/**
 * @brief Process an option tag
 *
 * @param docblock The documentation block
 * @param content The tag content
 * @return true if successful, false otherwise
 */
bool process_option_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Check if a tag is an option tag
 *
 * @param tag The tag name
 * @return true if the tag is an option tag, false otherwise
 */
bool is_option_tag(const char *tag);

/**
 * @brief Parse the option content to extract the option and its description
 * 
 * @param content The option content to parse
 * @param option Output parameter for the extracted option
 * @param description Output parameter for the extracted description
 * @param arg_spec Output parameter for the argument specification, if any
 * @return true if parsing was successful, false otherwise
 */
bool parse_option_content(const char *content, char **option, char **description, char **arg_spec);

#endif /* SHELLSCRIBE_OPTION_H */ 
