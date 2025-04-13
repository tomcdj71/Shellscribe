/**
 * @file argument.h
 * @brief Parser for function arguments in shell script documentation
 */

#ifndef SHELLSCRIBE_ARGUMENT_H
#define SHELLSCRIBE_ARGUMENT_H

#include "parsers/types.h"
#include "utils/config.h"
#include <stdbool.h>

/**
 * @brief Process an argument tag (@arg)
 *
 * Format: @arg $1 string Description of argument
 *
 * @param docblock The documentation block
 * @param content The argument tag content
 * @return true if successful, false otherwise
 */
bool process_argument_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Parses a parameter tag content (@param)
 *
 * Format: @param name Description of parameter
 *
 * @param docblock The documentation block
 * @param content The parameter tag content
 * @return true if successful, false otherwise
 */
bool process_parameter_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Check if a tag is an argument tag
 *
 * @param tag The tag name
 * @return true if the tag is an argument tag, false otherwise
 */
bool is_argument_tag(const char *tag);

#endif /* SHELLSCRIBE_ARGUMENT_H */ 
