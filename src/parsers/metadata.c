/**
 * @file metadata.c
 * @brief Implementation of metadata parsing
 *
 * This file provides functionality for parsing and processing file-level metadata
 * tags in documentation blocks. Metadata includes information about the file itself,
 * such as author, version, license, and copyright information. These tags typically
 * appear at the top of the file and apply to the entire document rather than
 * to a specific function.
 */

#include "parsers/metadata.h"
#include "utils/string.h"
#include "utils/debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Check if a tag is a file-level metadata tag
 * 
 * Determines whether a given tag name is a file-level metadata tag.
 * File-level metadata tags provide information about the file as a whole
 * rather than about a specific function.
 *
 * @param tag The tag name to check, without the '@' prefix
 * 
 * @return bool true if the tag is a file-level metadata tag, false otherwise or if tag is NULL
 *
 * @note This function is case-sensitive, so tags must exactly match one of the
 *       recognized forms
 * @note Recognized file-level tags include: file, version, author, license, copyright,
 *       since, description, package, module, link, repo, see, env, and skip
 */
bool is_file_level_tag(const char *tag) {
    if (tag == NULL) {
        return false;
    }
    const char *file_tags[] = {
        "file", "version", "author", "license", "copyright",
        "since", "description", "package", "module",
        "link", "repo", "see", "env", "skip",
        NULL
    };
    for (int i = 0; file_tags[i] != NULL; i++) {
        if (strcmp(tag, file_tags[i]) == 0) {
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Check if a tag is a file-level description
 * 
 * Determines whether a description tag should be treated as a file-level description
 * based on its position in the file. Description tags that appear early in the file
 * (typically within the first 10 lines) are considered file-level descriptions.
 *
 * @param tag The tag name to check, without the '@' prefix
 * @param line_number The line number where the tag appears in the file
 * 
 * @return bool true if the tag is a file-level description, false otherwise or if tag is NULL
 *
 * @note This function uses both the tag name and its position to determine if it's
 *       a file-level description
 * @note Only tags named "description" are considered, and they must appear within
 *       the first 10 lines of the file
 */
bool is_file_level_description(const char *tag, int line_number) {
    if (tag == NULL) {
        return false;
    }
    bool is_description_tag = (strcmp(tag, "description") == 0);
    bool is_early_in_file = (line_number < 10);
    
    return (is_description_tag && is_early_in_file);
}

/**
 * @brief Process a file metadata tag and add it to a documentation block
 * 
 * Parses a file-level metadata tag and stores it in the appropriate field
 * of the provided documentation block. Handles various metadata types including
 * file name, version, author, license, and description.
 *
 * @param docblock The documentation block to add the metadata to
 * @param tag The metadata tag name, without the '@' prefix
 * @param content The content of the metadata tag
 * 
 * @return bool true if the metadata was successfully processed and added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 *
 * @note Any existing value for the corresponding metadata field will be freed
 *       before the new value is assigned
 * @note Memory is allocated for the metadata value, which will be freed
 *       when the docblock is freed
 * @note The 'skip' tag is handled specially, setting a flag rather than storing content
 */
bool process_file_metadata_tag(shellscribe_docblock_t *docblock, const char *tag, const char *content) {
    if (docblock == NULL || tag == NULL || content == NULL) {
        return false;
    }

    struct {
        const char *tag_name;
        char **docblock_field;
    } tag_mappings[] = {
        {"file", &docblock->file_name},
        {"version", &docblock->version},
        {"author", &docblock->author},
        {"since", &docblock->author_contact},
        {"description", &docblock->description},
        {"brief", &docblock->brief},
        {"license", &docblock->license},
        {"copyright", &docblock->copyright},
        {NULL, NULL}
    };

    for (int i = 0; tag_mappings[i].tag_name != NULL; i++) {
        if (strcmp(tag, tag_mappings[i].tag_name) == 0) {
            free(*tag_mappings[i].docblock_field);
            *tag_mappings[i].docblock_field = string_duplicate(content);
            return true;
        }
    }

    if (strcmp(tag, "skip") == 0) {
        docblock->is_skipped = true;
        return true;
    }

    return false;
}

/**
 * @brief Process an alert tag and add it to a documentation block
 * 
 * Internal function that creates an alert entry from a tag name and content.
 * The tag name is converted to uppercase to standardize the alert type.
 *
 * @param docblock The documentation block to add the alert to
 * @param tag The alert type (e.g., "note", "warning")
 * @param content The alert message content
 * 
 * @return bool true if the alert was successfully processed and added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 *
 * @note The tag name is converted to uppercase to standardize alert types
 * @note Memory is allocated for the alert type and content, which will be freed
 *       when the docblock is freed
 * @note This is primarily an internal function used by other tag processors
 */
static bool metadata_process_alert_tag(shellscribe_docblock_t *docblock, const char *tag, const char *content) {
    if (docblock == NULL || tag == NULL || content == NULL) {
        return false;
    }
    char *alert_type = string_duplicate(tag);
    if (alert_type == NULL) {
        return false;
    }
    for (char *p = alert_type; *p; p++) {
        *p = toupper(*p);
    }
    shellscribe_alert_t *new_alerts = realloc(docblock->alerts, (docblock->alert_count + 1) * sizeof(shellscribe_alert_t));
    if (new_alerts == NULL) {
        free(alert_type);
        return false;
    }
    
    docblock->alerts = new_alerts;
    docblock->alerts[docblock->alert_count].type = alert_type;
    docblock->alerts[docblock->alert_count].content = string_duplicate(content);
    docblock->alert_count++;
    
    return true;
} 
