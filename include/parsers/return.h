/**
 * @file return.h
 * @brief Parser for return values in shell script documentation
 */

#ifndef SHELLSCRIBE_RETURN_H
#define SHELLSCRIBE_RETURN_H

#include "parsers/types.h"
#include <stdbool.h>

/**
 * @brief Process a return value tag (@return)
 *
 * @param docblock The documentation block
 * @param content The return tag content
 * @return true if successful, false otherwise
 */
bool process_return_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Process a returns value tag (@returns) - alias for @return
 *
 * @param docblock The documentation block
 * @param content The returns tag content
 * @return true if successful, false otherwise
 */
bool process_returns_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Check if a tag is a return-related tag
 *
 * @param tag The tag name
 * @return true if the tag is a return-related tag, false otherwise
 */
bool is_return_tag(const char *tag);

#endif /* SHELLSCRIBE_RETURN_H */ 
