/**
 * @file alias.c
 * @brief Implementation of alias tag parser
 *
 * This file provides functionality for parsing and processing alias tags
 * in documentation blocks. Aliases allow functions to be documented under
 * alternative names, which is useful for functions with multiple entry points
 * or backwards compatibility interfaces.
 */

#include "parsers/alias.h"
#include "utils/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Check if a tag is an alias tag
 *
 * Determines whether a given tag name is an alias tag. An alias tag
 * indicates an alternative name for a function that should be included
 * in the documentation.
 *
 * @param tag The tag name to check, without the '@' prefix
 *
 * @return bool true if the tag is an alias tag, false otherwise or if tag is NULL
 *
 * @note This function is case-sensitive, so the tag must exactly match "alias"
 */
bool is_alias_tag(const char *tag) {
    if (tag == NULL) {
        return false;
    }
    return (strcmp(tag, "alias") == 0);
}

/**
 * @brief Process an alias tag and store the alias in a documentation block
 *
 * Extracts the alias name from the tag content and stores it in the
 * provided documentation block. If the docblock already had an alias,
 * the previous value is freed before the new value is assigned.
 *
 * @param docblock The documentation block to add the alias to
 * @param content The content of the alias tag, containing the alias name
 *
 * @return bool true if the alias was successfully processed and added to the docblock,
 *              false if an error occurred (e.g., empty content, NULL parameters,
 *              or memory allocation failure)
 *
 * @note Only one alias can be stored per documentation block. If multiple
 *       alias tags are processed for the same block, only the last one is kept.
 * @note Memory is allocated for the alias string, which will be freed
 *       when the docblock is freed.
 */
bool process_alias_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL || *content == '\0') {
        return false;
    }
    free(docblock->alias);
    docblock->alias = string_duplicate(content);
    return (docblock->alias != NULL);
} 
