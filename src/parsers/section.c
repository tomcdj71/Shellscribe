/**
 * @file section.c
 * @brief Implementation of section tag parser
 *
 * This file provides functionality for parsing and processing section tags
 * in documentation blocks. Sections are used to organize documentation into
 * logical parts that improve readability by grouping related information
 * together. A section tag (@section) creates a new named section within the
 * documentation output.
 */

#include "parsers/section.h"
#include "utils/string.h"
#include "utils/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Check if a tag is a section tag
 *
 * Determines whether a given tag name is a section tag, which is used
 * to create distinct sections in the documentation output.
 *
 * @param tag The tag name to check, without the '@' prefix
 *
 * @return bool true if the tag is a section tag (specifically "section"),
 *              false otherwise or if tag is NULL
 *
 * @note This function is case-sensitive, so the tag must exactly match "section"
 */
bool is_section_tag(const char *tag) {
    if (tag == NULL) {
        return false;
    }
    
    return (strcmp(tag, "section") == 0);
}

/**
 * @brief Process a section tag (@section)
 *
 * Parses a section tag and adds the section information to the documentation block.
 * A section tag creates a new named section within the generated documentation.
 * The first word after the @section tag is treated as the section name, and
 * the remaining content is the section description.
 *
 * @param docblock The documentation block to add the section to
 * @param content The content of the section tag, containing the section name 
 *                and description
 *
 * @return bool true if the section was successfully processed and added to the
 *              docblock, false if an error occurred or if required parameters are NULL
 *
 * @note The expected format is "name description" where name is the section name
 *       (without spaces) and everything after the first space is the description
 * @note If the section content doesn't contain a space, the whole content is used
 *       as the section name with an empty description
 * @note Memory is allocated for the section information, which will be freed
 *       when the docblock is freed
 * @note Multiple section tags can be used in a single docblock to create multiple
 *       sections within the documentation
 */
bool process_section_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    const char *name_end = content;
    while (*name_end && *name_end != ' ') {
        name_end++;
    }
    if (name_end == content) {
        return false;
    }
    size_t name_len = name_end - content;
    char *name = (char *)shell_malloc(name_len + 1);
    if (name == NULL) {
        return false;
    }
    strncpy(name, content, name_len);
    name[name_len] = '\0';
    const char *description = (*name_end) ? name_end + 1 : "";
    if (docblock->section == NULL) {
        docblock->section = shell_malloc(sizeof(shellscribe_section_t));
        if (docblock->section == NULL) {
            void *name_ptr = name;
            shell_free(&name_ptr);
            return false;
        }
    } else {
        void *old_name = docblock->section->name;
        void *old_description = docblock->section->description;
        shell_free(&old_name);
        shell_free(&old_description);
    }
    docblock->section->name = name;
    docblock->section->description = string_duplicate(description);
    if (docblock->section->description == NULL) {
        void *name_ptr = name;
        shell_free(&name_ptr);
        return false;
    }
    
    return true;
} 
