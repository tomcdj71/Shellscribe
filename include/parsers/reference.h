/**
 * @file reference.h
 * @brief Parser for @see tag in shell script documentation
 */

#ifndef SHELLSCRIBE_REFERENCE_H
#define SHELLSCRIBE_REFERENCE_H

#include "parsers/types.h"
#include <stdbool.h>

/**
 * @brief Process a see tag
 *
 * @param docblock The documentation block
 * @param content The tag content
 * @return true if successful, false otherwise
 */
bool process_see_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Check if a tag is a see tag
 *
 * @param tag The tag name
 * @return true if the tag is a see tag, false otherwise
 */
bool is_see_tag(const char *tag);

/**
 * @brief Parse the see content to extract the reference name and URL (if any)
 * 
 * @param content The see tag content to parse
 * @param name Output parameter for the extracted reference name
 * @param url Output parameter for the extracted URL, if any
 * @param is_internal Output parameter indicating if this is an internal reference
 * @return true if parsing was successful, false otherwise
 */
bool parse_see_content(const char *content, char **name, char **url, bool *is_internal);

#endif /* SHELLSCRIBE_REFERENCE_H */ 
