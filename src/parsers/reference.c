/**
 * @file reference.c
 * @brief Implementation of reference tag parser (@see)
 *
 * This file provides functionality for parsing and processing reference tags
 * (denoted by @see) in documentation blocks. References provide cross-references
 * to other functions, files, or external resources that are related to the
 * current documentation. References can be internal (within the same codebase)
 * or external (to websites or other documentation).
 */

#include "parsers/reference.h"
#include "utils/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Check if a tag is a see tag
 * 
 * Determines whether a given tag name is a reference tag (specifically "see").
 * Reference tags are used to link to related documentation, both internal
 * and external.
 *
 * @param tag The tag name to check, without the '@' prefix
 * 
 * @return bool true if the tag is a see tag, false otherwise or if tag is NULL
 *
 * @note This function is case-sensitive, so the tag must exactly match "see"
 */
bool is_see_tag(const char *tag) {
    if (tag == NULL) {
        return false;
    }
    
    return (strcmp(tag, "see") == 0);
}

/**
 * @brief Parse the see content to extract the reference name and URL (if any)
 * 
 * Parses the content of a see tag to extract the reference name and URL.
 * Supports two formats:
 * 1. Simple format: "function_name" - for internal references
 * 2. Markdown link format: "[Display Name](URL)" - for external references
 *
 * @param content The content of the see tag to parse
 * @param name Pointer to a string pointer that will store the extracted name
 * @param url Pointer to a string pointer that will store the extracted URL
 * @param is_internal Pointer to a boolean that will indicate if it's an internal reference
 * 
 * @return bool true if parsing was successful and the reference was extracted,
 *              false if an error occurred or if required parameters are NULL
 *
 * @note Memory is allocated for the name and url (if present), which must be freed
 *       by the caller when no longer needed
 * @note If the reference is internal (simple format), url will be set to NULL and
 *       is_internal will be set to true
 * @note If the reference is external (Markdown link format), both name and url
 *       will be populated and is_internal will be set to false
 */
bool parse_see_content(const char *content, char **name, char **url, bool *is_internal) {
    if (content == NULL || name == NULL || url == NULL || is_internal == NULL) {
        return false;
    }
    
    *name = NULL;
    *url = NULL;
    *is_internal = true;
    while (isspace(*content)) {
        content++;
    }
    if (*content == '\0') {
        return false;
    }
    const char *open_bracket = strchr(content, '[');
    const char *close_bracket = strchr(content, ']');
    const char *open_paren = strchr(content, '(');
    const char *close_paren = strchr(content, ')');
    if (open_bracket != NULL && close_bracket != NULL &&
        open_paren != NULL && close_paren != NULL &&
        open_bracket < close_bracket && close_bracket < open_paren && open_paren < close_paren) {
        size_t name_len = close_bracket - open_bracket - 1;
        *name = malloc(name_len + 1);
        if (*name == NULL) {
            return false;
        }
        strncpy(*name, open_bracket + 1, name_len);
        (*name)[name_len] = '\0';
        size_t url_len = close_paren - open_paren - 1;
        *url = malloc(url_len + 1);
        if (*url == NULL) {
            free(*name);
            *name = NULL;
            return false;
        }
        strncpy(*url, open_paren + 1, url_len);
        (*url)[url_len] = '\0';
        *is_internal = false;
    } else {
        *name = string_duplicate(content);
        *url = NULL;
        *is_internal = true;
    }
    
    return true;
}

/**
 * @brief Process a see tag and add it to a documentation block
 * 
 * Parses a see tag's content, extracts the reference information,
 * and adds it to the provided documentation block. Supports both
 * internal references (to other functions) and external references
 * (to websites or external documentation).
 *
 * @param docblock The documentation block to add the reference to
 * @param content The content of the see tag
 * 
 * @return bool true if the reference was successfully processed and added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 *
 * @note This function uses parse_see_content to extract the reference components
 * @note Memory is allocated for the reference information, which will be freed
 *       when the docblock is freed
 * @note References are stored in the see_also array of the docblock structure
 * 
 * @see parse_see_content
 */
bool process_see_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    
    char *name = NULL;
    char *url = NULL;
    bool is_internal = true;
    if (!parse_see_content(content, &name, &url, &is_internal)) {
        return false;
    }
    shellscribe_see_also_t *new_see_also = realloc(docblock->see_also, (docblock->see_also_count + 1) * sizeof(shellscribe_see_also_t));
    if (new_see_also == NULL) {
        free(name);
        free(url);
        return false;
    }
    docblock->see_also = new_see_also;
    docblock->see_also[docblock->see_also_count].name = name;
    docblock->see_also[docblock->see_also_count].url = url;
    docblock->see_also[docblock->see_also_count].is_internal = is_internal;
    docblock->see_also_count++;
    
    return true;
} 
