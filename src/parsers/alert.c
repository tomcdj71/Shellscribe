/**
 * @file alert.c
 * @brief Implementation of GitHub alert tags parser (@note, @tip, @important, @warning, @caution)
 *
 * This file provides functions for parsing and processing special alert tags
 * that are rendered as GitHub-style alert boxes in the generated documentation.
 * These tags provide visual emphasis for important information, warnings, tips,
 * and other noteworthy content within the documentation.
 */

#include "parsers/alert.h"
#include "utils/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Check if a tag is a GitHub alert tag
 *
 * Determines whether a given tag name represents a GitHub alert type.
 * The following tags are recognized as alert tags:
 * - note: General information
 * - tip/hint: Helpful advice
 * - important: Critical information
 * - warning/caution: Potential problems or pitfalls
 * - info: Informational content
 * - danger: Serious issues or dangerous operations
 *
 * @param tag The tag name to check, without the '@' prefix
 *
 * @return bool true if the tag is a recognized alert tag, false otherwise or if tag is NULL
 *
 * @note This function is case-sensitive, so tags must exactly match one of the
 *       recognized types.
 */
bool is_alert_tag(const char *tag) {
    if (tag == NULL) {
        return false;
    }
    return (strcmp(tag, "note") == 0 ||
            strcmp(tag, "tip") == 0 ||
            strcmp(tag, "important") == 0 ||
            strcmp(tag, "warning") == 0 ||
            strcmp(tag, "caution") == 0 ||
            strcmp(tag, "info") == 0 ||
            strcmp(tag, "danger") == 0 ||
            strcmp(tag, "hint") == 0);
}

/**
 * @brief Get the standardized alert type code based on the tag name
 *
 * Maps the various alert tag names to their corresponding standardized
 * type codes. This normalization ensures consistent rendering regardless
 * of which synonym was used in the source documentation.
 *
 * @param tag The alert tag name to convert to a type code
 *
 * @return const char* The standardized type code as an uppercase string.
 *                    Returns "NOTE" if tag is NULL or not recognized.
 *
 * @note Some tags are mapped to the same type code:
 *       - "hint" is mapped to "TIP"
 *       - Any unrecognized tag is mapped to "NOTE"
 * @note The returned string is a static constant and should not be freed.
 */
const char *get_alert_type(const char *tag) {
    if (tag == NULL) {
        return "NOTE";
    }
    
    if (strcmp(tag, "note") == 0) {
        return "NOTE";
    } else if (strcmp(tag, "tip") == 0) {
        return "TIP";
    } else if (strcmp(tag, "important") == 0) {
        return "IMPORTANT";
    } else if (strcmp(tag, "warning") == 0) {
        return "WARNING";
    } else if (strcmp(tag, "caution") == 0) {
        return "CAUTION";
    } else if (strcmp(tag, "info") == 0) {
        return "INFO";
    } else if (strcmp(tag, "danger") == 0) {
        return "DANGER";
    } else if (strcmp(tag, "hint") == 0) {
        return "TIP";
    }
    
    return "NOTE";
}

/**
 * @brief Process an alert tag and add it to a documentation block
 *
 * Parses an alert tag, converts it to the appropriate type using get_alert_type(),
 * and adds it to the provided documentation block. The alert will later be
 * rendered as a GitHub-style alert box in the generated documentation.
 *
 * @param docblock The documentation block to add the alert to
 * @param tag The specific alert tag type (e.g., "note", "warning")
 * @param content The content/message of the alert
 *
 * @return bool true if the alert was successfully processed and added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 *
 * @note Memory is allocated for the alert type and content, which will be
 *       freed when the docblock is freed.
 * @note If memory allocation fails, the function returns false and no alert
 *       is added to the docblock.
 *
 * @see get_alert_type
 * @see is_alert_tag
 */
bool process_alert_tag(shellscribe_docblock_t *docblock, const char *tag, const char *content) {
    if (docblock == NULL || tag == NULL || content == NULL) {
        return false;
    }
    const char *alert_type = get_alert_type(tag);
    shellscribe_alert_t *new_alerts = realloc(docblock->alerts, (docblock->alert_count + 1) * sizeof(shellscribe_alert_t));
    if (new_alerts == NULL) {
        return false;
    }
    docblock->alerts = new_alerts;
    docblock->alerts[docblock->alert_count].type = string_duplicate(alert_type);
    docblock->alerts[docblock->alert_count].content = string_duplicate(content);
    docblock->alert_count++;
    
    return true;
} 
