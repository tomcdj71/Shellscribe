/**
 * @file description.c
 * @brief Implementation of description tag parser
 *
 * This file provides functionality for parsing and processing description tags
 * in documentation blocks. Descriptions are a fundamental part of documentation,
 * providing detailed explanations of files, functions, and other elements.
 * The parser handles both file-level and function-level descriptions.
 */

#include "parsers/description.h"
#include "utils/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Check if a tag is a description tag
 * 
 * Determines whether a given tag name is a description tag.
 * Description tags provide detailed explanations of the documented element.
 *
 * @param tag The tag name to check, without the '@' prefix
 * 
 * @return bool true if the tag is a description tag, false otherwise or if tag is NULL
 *
 * @note This function is case-sensitive, so the tag must exactly match "description"
 */
bool is_description_tag(const char *tag) {
    if (tag == NULL) {
        return false;
    }
    
    return (strcmp(tag, "description") == 0);
}

/**
 * @brief Process a description tag and add it to a documentation block
 * 
 * Parses a description tag's content and adds it to the appropriate field
 * in the provided documentation block. The function handles both file-level
 * and function-level descriptions, determining which to use based on whether
 * a function name has been set in the docblock.
 *
 * If a description already exists for the target field, the new content
 * is appended to the existing description with a newline separator.
 *
 * @param docblock The documentation block to add the description to
 * @param content The content of the description tag
 * 
 * @return bool true if the description was successfully processed and added
 *              to the docblock, false if an error occurred or if required
 *              parameters are NULL
 *
 * @note Memory is allocated for the description string, which will be freed
 *       when the docblock is freed.
 * @note If the docblock has a function_name set, the description is added to
 *       function_description; otherwise, it's added to the general description field.
 * @note Multiple description tags for the same element are concatenated with newlines.
 */
bool process_description_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    if (docblock->function_name == NULL) {
        if (docblock->description == NULL) {
            docblock->description = string_duplicate(content);
        } else {
            char *new_desc = string_concat(docblock->description, "\n");
            free(docblock->description);
            char *temp = new_desc;
            new_desc = string_concat(temp, content);
            free(temp);
            
            docblock->description = new_desc;
        }
    } else {
        if (docblock->function_description == NULL) {
            docblock->function_description = string_duplicate(content);
        } else {
            char *new_desc = string_concat(docblock->function_description, "\n");
            free(docblock->function_description);
            char *temp = new_desc;
            new_desc = string_concat(temp, content);
            free(temp);
            
            docblock->function_description = new_desc;
        }
    }
    
    return true;
}

/**
 * @brief Finalize description processing for a documentation block
 * 
 * Ensures that the section description is set if a section exists
 * within the docblock but doesn't yet have a description. This is a
 * cleanup step typically called after all tags have been processed.
 *
 * @param docblock The documentation block to finalize
 * 
 * @return bool true if the finalization was successful or if no action was needed,
 *              false if an error occurred or if the docblock is NULL
 *
 * @note If the docblock has a section and that section doesn't have a description set,
 *       this function will copy the main description to the section description.
 * @note Memory is allocated for the section description, which will be freed
 *       when the docblock is freed.
 */
bool finalize_description(shellscribe_docblock_t *docblock) {
    if (docblock == NULL) {
        return false;
    }
    
    if (docblock->section != NULL && docblock->section->description == NULL) {
        if (docblock->description != NULL) {
            docblock->section->description = string_duplicate(docblock->description);
        }
    }
    
    return true;
} 
