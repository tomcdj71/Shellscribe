/**
 * @file variable.c
 * @brief Implementation of variable tag parser for @set tag
 *
 * This file provides functionality for parsing and processing variable-related tags
 * in documentation blocks. The main tag supported is @set, which is used to document
 * global variables or environment variables that are set by a script or function.
 * These tags help users understand what variables are modified and how they are used.
 */

#include "parsers/variable.h"
#include "utils/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Check if a tag is a set tag
 * 
 * Determines whether a given tag name is a set tag, which is used to document
 * variables that are set by a function or script.
 *
 * @param tag The tag name to check, without the '@' prefix
 * 
 * @return bool true if the tag is a set tag (specifically "set"),
 *              false otherwise or if tag is NULL
 *
 * @note This function is case-sensitive, so the tag must exactly match "set"
 */
bool is_set_tag(const char *tag) {
    if (tag == NULL) {
        return false;
    }
    
    return (strcmp(tag, "set") == 0);
}

/**
 * @brief Parse the set content to extract the variable name, type and description
 * 
 * Parses the content of a set tag to extract the variable name, type, and description.
 * The expected format is "name type description" where type and description are optional.
 *
 * @param content The content of the set tag to parse
 * @param name Pointer to a string pointer that will store the extracted variable name
 * @param type Pointer to a string pointer that will store the extracted variable type
 * @param description Pointer to a string pointer that will store the extracted description
 * 
 * @return bool true if parsing was successful and the variable information was extracted,
 *              false if an error occurred or if required parameters are NULL
 *
 * @note Memory is allocated for the name, type, and description, which must be freed
 *       by the caller when no longer needed
 * @note If the content doesn't contain a type or description, empty strings are used
 * @note Leading whitespace in the content is skipped
 */
bool parse_set_content(const char *content, char **name, char **type, char **description) {
    if (content == NULL || name == NULL || type == NULL || description == NULL) {
        return false;
    }
    
    *name = NULL;
    *type = NULL;
    *description = NULL;
    while (isspace(*content)) {
        content++;
    }
    if (*content == '\0') {
        return false;
    }
    const char *name_start = content;
    const char *name_end = strchr(content, ' ');
    if (name_end == NULL) {
        *name = string_duplicate(content);
        *type = string_duplicate("");
        *description = string_duplicate("");
        return true;
    }
    size_t name_len = name_end - name_start;
    *name = malloc(name_len + 1);
    if (*name == NULL) {
        return false;
    }
    strncpy(*name, name_start, name_len);
    (*name)[name_len] = '\0';
    const char *type_start = name_end + 1;
    while (isspace(*type_start)) {
        type_start++;
    }
    const char *type_end = strchr(type_start, ' ');
    if (type_end == NULL) {
        *type = string_duplicate(type_start);
        *description = string_duplicate("");
        return true;
    }
    size_t type_len = type_end - type_start;
    *type = malloc(type_len + 1);
    if (*type == NULL) {
        free(*name);
        *name = NULL;
        return false;
    }
    strncpy(*type, type_start, type_len);
    (*type)[type_len] = '\0';
    const char *desc_start = type_end + 1;
    while (isspace(*desc_start)) {
        desc_start++;
    }
    *description = string_duplicate(desc_start);
    return true;
}

/**
 * @brief Process a set tag and add it to a documentation block
 * 
 * Processes a set tag and adds the variable information to the specified
 * documentation block. The set tag is used to document variables that are
 * set by a function or script.
 *
 * @param docblock The documentation block to add the variable to
 * @param content The content of the set tag
 * 
 * @return bool true if the variable was successfully processed and added to the
 *              docblock, false if an error occurred or if required parameters are NULL
 *
 * @note This function uses parse_set_content to extract the variable components
 * @note Memory is allocated for the variable information, which will be freed
 *       when the docblock is freed
 * @note Variables are stored in the set_vars array of the docblock structure
 * @note By default, variables are marked as not readonly, and no default value is set
 * 
 * @see parse_set_content
 */
bool process_set_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    char *name = NULL;
    char *type = NULL;
    char *description = NULL;
    if (!parse_set_content(content, &name, &type, &description)) {
        return false;
    }
    shellscribe_global_var_t *new_vars = realloc(docblock->set_vars, (docblock->set_var_count + 1) * sizeof(shellscribe_global_var_t));
    if (new_vars == NULL) {
        free(name);
        free(type);
        free(description);
        return false;
    }
    docblock->set_vars = new_vars;
    docblock->set_vars[docblock->set_var_count].name = name;
    docblock->set_vars[docblock->set_var_count].type = type;
    docblock->set_vars[docblock->set_var_count].description = description;
    docblock->set_vars[docblock->set_var_count].default_value = NULL;
    docblock->set_vars[docblock->set_var_count].is_readonly = false;
    docblock->set_var_count++;
    
    return true;
} 
