/**
 * @file alert.h
 * @brief Parser for GitHub alert tags (@note, @tip, @important, @warning, @caution) in shell script documentation
 */

#ifndef SHELLSCRIBE_ALERT_H
#define SHELLSCRIBE_ALERT_H

#include "parsers/types.h"
#include <stdbool.h>

/**
 * @brief Process an alert tag (note, tip, important, warning, caution)
 *
 * @param docblock The documentation block
 * @param tag The alert tag type
 * @param content The tag content
 * @return true if successful, false otherwise
 */
bool process_alert_tag(shellscribe_docblock_t *docblock, const char *tag, const char *content);

/**
 * @brief Check if a tag is a GitHub alert tag
 *
 * @param tag The tag name
 * @return true if the tag is a GitHub alert tag, false otherwise
 */
bool is_alert_tag(const char *tag);

/**
 * @brief Get the alert type code based on the tag name
 * 
 * @param tag The tag name
 * @return const char* The alert type code (NOTE, TIP, IMPORTANT, WARNING, CAUTION)
 */
const char *get_alert_type(const char *tag);

#endif /* SHELLSCRIBE_ALERT_H */ 
