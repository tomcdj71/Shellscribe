/**
 * @file internal.h
 * @brief Parser for @internal tag in shell script documentation
 */

#ifndef SHELLSCRIBE_INTERNAL_H
#define SHELLSCRIBE_INTERNAL_H

#include "parsers/types.h"
#include <stdbool.h>

/**
 * @brief Process an internal tag
 *
 * @param docblock The documentation block
 * @param content The tag content (unused)
 * @return true if successful, false otherwise
 */
bool process_internal_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Check if a tag is an internal tag
 *
 * @param tag The tag name
 * @return true if the tag is an internal tag, false otherwise
 */
bool is_internal_tag(const char *tag);

#endif /* SHELLSCRIBE_INTERNAL_H */ 
