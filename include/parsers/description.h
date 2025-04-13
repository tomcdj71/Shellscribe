/**
 * @file description.h
 * @brief Parser for @description tag in shell script documentation
 */

#ifndef SHELLSCRIBE_DESCRIPTION_H
#define SHELLSCRIBE_DESCRIPTION_H

#include "parsers/types.h"
#include <stdbool.h>

/**
 * @brief Process a description tag
 *
 * @param docblock The documentation block
 * @param content The tag content
 * @return true if successful, false otherwise
 */
bool process_description_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Check if a tag is a description tag
 *
 * @param tag The tag name
 * @return true if the tag is a description tag, false otherwise
 */
bool is_description_tag(const char *tag);

/**
 * @brief Finalize description processing
 * 
 * @param docblock The documentation block
 * @return true if successful, false otherwise
 */
bool finalize_description(shellscribe_docblock_t *docblock);

#endif /* SHELLSCRIBE_DESCRIPTION_H */ 
