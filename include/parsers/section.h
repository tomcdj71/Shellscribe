/**
 * @file section.h
 * @brief Parser for @section tag in shell script documentation
 */

#ifndef SHELLSCRIBE_SECTION_H
#define SHELLSCRIBE_SECTION_H

#include "parsers/types.h"
#include <stdbool.h>

/**
 * @brief Process a section tag
 *
 * @param docblock The documentation block
 * @param content The tag content
 * @return true if successful, false otherwise
 */
bool process_section_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Check if a tag is a section tag
 *
 * @param tag The tag name
 * @return true if the tag is a section tag, false otherwise
 */
bool is_section_tag(const char *tag);

#endif /* SHELLSCRIBE_SECTION_H */ 
