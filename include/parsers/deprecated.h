/**
 * @file deprecated.h
 * @brief Parser for @deprecated, @replacement, and @eol tags in shell script documentation
 */

#ifndef SHELLSCRIBE_DEPRECATED_H
#define SHELLSCRIBE_DEPRECATED_H

#include "parsers/types.h"
#include <stdbool.h>

/**
 * @brief Process a deprecated tag
 *
 * @param docblock The documentation block
 * @param content The tag content
 * @return true if successful, false otherwise
 */
bool process_deprecated_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Process a replacement tag
 *
 * @param docblock The documentation block
 * @param content The tag content
 * @return true if successful, false otherwise
 */
bool process_replacement_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Process an eol (end-of-life) tag
 *
 * @param docblock The documentation block
 * @param content The tag content
 * @return true if successful, false otherwise
 */
bool process_eol_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Check if a tag is a deprecation-related tag
 *
 * @param tag The tag name
 * @return true if the tag is a deprecation-related tag, false otherwise
 */
bool is_deprecation_tag(const char *tag);

#endif /* SHELLSCRIBE_DEPRECATED_H */ 
