/**
 * @file internal.c
 * @brief Implementation of internal tag parser
 *
 * This file provides functionality for parsing and processing internal tags
 * in documentation blocks. Internal tags mark functions that are intended for
 * internal use only and should not be exposed in the public API documentation.
 * This helps maintain a clean separation between public and private interfaces.
 */

#include "parsers/internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Check if a tag is an internal tag
 * 
 * Determines whether a given tag name is an internal tag.
 * Internal tags mark functions that are not part of the public API
 * and should typically be excluded from generated documentation or
 * marked with a special indicator when included.
 *
 * @param tag The tag name to check, without the '@' prefix
 * 
 * @return bool true if the tag is an internal tag, false otherwise or if tag is NULL
 *
 * @note This function is case-sensitive, so the tag must exactly match "internal"
 */
bool is_internal_tag(const char *tag) {
    if (tag == NULL) {
        return false;
    }
    
    return (strcmp(tag, "internal") == 0);
}

/**
 * @brief Process an internal tag and mark a function as internal
 * 
 * Sets the is_internal flag on a documentation block to indicate that
 * the associated function is for internal use only. The content of the
 * tag is typically ignored as the presence of the tag alone is sufficient.
 *
 * @param docblock The documentation block to mark as internal
 * @param content The content of the internal tag (typically ignored)
 * 
 * @return bool true if the internal tag was successfully processed and the
 *              docblock was marked as internal, false if the docblock is NULL
 *
 * @note When a function is marked as internal, documentation generators
 *       may choose to exclude it from the generated output or mark it
 *       with a special indicator
 * @note The content parameter is typically ignored, as the presence of
 *       the tag alone is sufficient to mark a function as internal
 */
bool process_internal_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL) {
        return false;
    }
    docblock->is_internal = true;
    
    return true;
} 
