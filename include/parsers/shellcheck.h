/**
 * @file shellcheck.h
 * @brief Parser for shellcheck directives in shell script documentation
 */

#ifndef SHELLSCRIBE_SHELLCHECK_H
#define SHELLSCRIBE_SHELLCHECK_H

#include <stdbool.h>
#include "parsers/types.h"

/**
 * @brief Check if a line contains a shellcheck directive
 * 
 * @param line The line to check
 * @return true if the line contains a shellcheck directive, false otherwise
 */
bool is_shellcheck_directive(const char *line);

/**
 * @brief Extract a shellcheck directive from a line
 * 
 * @param line The line containing the shellcheck directive
 * @return char* The extracted directive, or NULL if not found
 */
char *extract_shellcheck_directive(const char *line);

/**
 * @brief Add a shellcheck directive to a docblock
 * 
 * @param docblock The docblock to add the directive to
 * @param directive The directive to add
 * @return true if successful, false otherwise
 */
bool add_shellcheck_directive(shellscribe_docblock_t *docblock, const char *directive);

/**
 * @brief Process a line to check for shellcheck directives
 * 
 * @param docblock The docblock to update
 * @param line The line to check
 * @return true if a directive was found and processed, false otherwise
 */
bool process_shellcheck_line(shellscribe_docblock_t *docblock, const char *line);

#endif /* SHELLSCRIBE_SHELLCHECK_H */ 
