/**
 * @file metadata.h
 * @brief Parser for file metadata in shell script documentation
 */

#ifndef SHELLSCRIBE_METADATA_H
#define SHELLSCRIBE_METADATA_H

#include "parsers/types.h"
#include <stdbool.h>

/**
 * @brief Check if a tag is a file-level metadata tag
 *
 * @param tag The tag name
 * @return true if the tag is a file-level tag, false otherwise
 */
bool is_file_level_tag(const char *tag);

/**
 * @brief Check if a tag is a file-level description tag
 *
 * @param tag The tag name
 * @param line_number The line number
 * @return true if the tag is a file-level description tag, false otherwise
 */
bool is_file_level_description(const char *tag, int line_number);

/**
 * @brief Process a file-level metadata tag
 *
 * @param docblock The documentation block
 * @param tag The tag name
 * @param content The tag content
 * @return true if successful, false otherwise
 */
bool process_file_metadata_tag(shellscribe_docblock_t *docblock, const char *tag, const char *content);

#endif /* SHELLSCRIBE_METADATA_H */ 
