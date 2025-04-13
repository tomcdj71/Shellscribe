/**
 * @file alias.h
 * @brief Parser for @alias tag in shell script documentation
 */

#ifndef SHELLSCRIBE_ALIAS_H
#define SHELLSCRIBE_ALIAS_H

#include "parsers/types.h"
#include <stdbool.h>

/**
 * @brief Process an alias tag
 *
 * @param docblock The documentation block
 * @param content The tag content
 * @return true if successful, false otherwise
 */
bool process_alias_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Check if a tag is an alias tag
 *
 * @param tag The tag name
 * @return true if the tag is an alias tag, false otherwise
 */
bool is_alias_tag(const char *tag);

#endif /* SHELLSCRIBE_ALIAS_H */ 
