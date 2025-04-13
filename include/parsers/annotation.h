/**
 * @file annotation.h
 * @brief Special annotation parsing for shell script documentation
 */

#ifndef SHELLSCRIBE_ANNOTATION_H
#define SHELLSCRIBE_ANNOTATION_H

#include <stdbool.h>
#include "parsers/types.h"

/**
 * @brief Check if a line contains a special annotation
 *
 * @param line The line to check
 * @return true if the line contains a special annotation, false otherwise
 */
bool is_special_annotation(const char *line);

/**
 * @brief Process a special annotation in a line
 *
 * @param line The line containing a special annotation
 * @param data User data for the callback
 * @return true if the annotation was processed, false otherwise
 */
bool process_special_annotation(const char *line, void *data);

/**
 * @brief Process a warning tag
 *
 * @param docblock The documentation block
 * @param content The warning content
 * @return true if successful, false otherwise
 */
bool process_warning_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Process a dependency tag
 *
 * @param docblock The documentation block
 * @param content The dependency content
 * @return true if successful, false otherwise
 */
bool process_dependency_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Process an internal call tag
 *
 * @param docblock The documentation block
 * @param content The internal call content
 * @return true if successful, false otherwise
 */
bool process_internal_call_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Process a required dependency tag (@requires)
 *
 * @param docblock The documentation block
 * @param content The required dependency content
 * @return true if successful, false otherwise
 */
bool process_requires_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Process a used-by tag (@used-by)
 *
 * @param docblock The documentation block
 * @param content The used-by content
 * @return true if successful, false otherwise
 */
bool process_used_by_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Process an external call tag (@calls)
 *
 * @param docblock The documentation block
 * @param content The external call content
 * @return true if successful, false otherwise
 */
bool process_calls_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Process a provided service tag (@provides)
 *
 * @param docblock The documentation block
 * @param content The provided service content
 * @return true if successful, false otherwise
 */
bool process_provides_tag(shellscribe_docblock_t *docblock, const char *content);

#endif /* SHELLSCRIBE_ANNOTATION_H */ 
