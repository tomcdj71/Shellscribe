/**
 * @file shellcheck.c
 * @brief Implementation of shellcheck directive parser
 *
 * This file provides functionality for parsing and processing shellcheck
 * directives in documentation blocks. Shellcheck directives are special
 * comments that control the behavior of the shellcheck static analysis tool
 * when examining shell scripts. This parser identifies and extracts these
 * directives so they can be included in the generated output.
 */

#define _GNU_SOURCE

#include "parsers/types.h"
#include "utils/string.h"
#include "utils/debug.h"
#include "utils/memory.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

/**
 * @brief Check if a line contains a shellcheck directive
 *
 * Determines whether a given line of text contains a shellcheck directive,
 * which typically has the format "# shellcheck disable=SC2034" or similar.
 *
 * @param line The line of text to check for a shellcheck directive
 *
 * @return bool true if the line contains a shellcheck directive,
 *              false otherwise or if line is NULL
 *
 * @note This function looks for the pattern of a shell comment followed by
 *       the word "shellcheck" (case-insensitive)
 * @note The line can have leading whitespace before the comment
 */
bool is_shellcheck_directive(const char *line) {
    if (line == NULL) {
        return false;
    }
    while (*line && isspace(*line)) {
        line++;
    }
    if (*line != '#') {
        return false;
    }
    line++;
    while (*line && isspace(*line)) {
        line++;
    }
    return (strncasecmp(line, "shellcheck", 10) == 0);
}

/**
 * @brief Extract shellcheck directive from a line
 *
 * Extracts the actual shellcheck directive from a line of text,
 * removing any leading comment symbol and whitespace.
 *
 * @param line The line of text containing a shellcheck directive
 *
 * @return char* A newly allocated string containing the extracted directive
 *               or NULL if extraction failed or if line is NULL
 *
 * @note The returned string must be freed by the caller
 * @note This function uses shell_malloc from the memory utilities
 * @note The extraction preserves the entire directive including the
 *       "shellcheck" keyword and all parameters
 */
char *extract_shellcheck_directive(const char *line) {
    if (line == NULL) {
        return NULL;
    }
    while (*line && isspace(*line)) {
        line++;
    }
    if (*line == '#') {
        line++;
        while (*line && isspace(*line)) {
            line++;
        }
    }
    
    return string_duplicate(line);
}

/**
 * @brief Parse a shellcheck directive to extract code and reason
 * 
 * This function parses a shellcheck directive like "shellcheck disable=SC2034 # Reason"
 * to extract the code (SC2034) and the reason (if provided after a # or comment).
 *
 * @param directive The full shellcheck directive to parse
 * @param code Pointer to store the extracted code
 * @param reason Pointer to store the extracted reason
 *
 * @return bool true if parsing was successful, false otherwise
 *
 * @note Both code and reason will be newly allocated strings that must be freed by the caller
 * @note If no reason is provided, *reason will be NULL
 */
bool parse_shellcheck_directive(const char *directive, char **code, char **reason) {
    if (directive == NULL || code == NULL || reason == NULL) {
        return false;
    }
    
    *code = NULL;
    *reason = NULL;
    const char *sc_start = NULL;
    const char *disable_pattern = strstr(directive, "disable=");
    const char *enable_pattern = strstr(directive, "enable=");
    if (disable_pattern) {
        sc_start = disable_pattern + 8;
    } else if (enable_pattern) {
        sc_start = enable_pattern + 7;
    }
    if (!sc_start) {
        *code = string_duplicate(directive);
        return true;
    }    const char *code_end = sc_start;
    while (*code_end && !isspace(*code_end) && *code_end != '#') {
        code_end++;
    }
    size_t code_len = code_end - sc_start;
    *code = (char *)shell_malloc(code_len + 1);
    if (*code == NULL) {
        return false;
    }
    strncpy(*code, sc_start, code_len);
    (*code)[code_len] = '\0';
    const char *reason_start = strchr(code_end, '#');
    if (reason_start) {
        reason_start++;
        while (*reason_start && isspace(*reason_start)) {
            reason_start++;
        }
        if (*reason_start) {
            *reason = string_duplicate(reason_start);
        }
    }
    
    return true;
}

/**
 * @brief Add a shellcheck directive to a documentation block
 *
 * Adds a shellcheck directive to the specified documentation block by
 * adding it to the directives array. The directive is parsed to extract
 * the code and reason.
 *
 * @param docblock The documentation block to add the directive to
 * @param directive The shellcheck directive to add
 *
 * @return bool true if the directive was successfully added,
 *              false if memory allocation failed or parameters are NULL
 *
 * @note This function allocates memory for the directive details, which will be freed
 *       when the docblock is freed
 * @note The directive is parsed to extract the code (e.g., SC2034) and reason
 */
bool add_shellcheck_directive(shellscribe_docblock_t *docblock, const char *directive) {
    if (docblock == NULL || directive == NULL) {
        return false;
    }
    shellscribe_shellcheck_t *new_directives = shell_realloc(docblock->shellcheck_directives, 
                               (docblock->shellcheck_count + 1) * sizeof(shellscribe_shellcheck_t));
    if (new_directives == NULL) {
        return false;
    }
    docblock->shellcheck_directives = new_directives;
    shellscribe_shellcheck_t *new_entry = &docblock->shellcheck_directives[docblock->shellcheck_count];
    memset(new_entry, 0, sizeof(shellscribe_shellcheck_t));
    char *code = NULL;
    char *reason = NULL;
    if (!parse_shellcheck_directive(directive, &code, &reason)) {
        return false;
    }
    new_entry->directive = string_duplicate(directive);
    new_entry->code = code;
    new_entry->reason = reason;
    if (new_entry->directive == NULL) {
        void *code_ptr = code;
        void *reason_ptr = reason;
        shell_free(&code_ptr);
        shell_free(&reason_ptr);
        return false;
    }
    docblock->shellcheck_count++;
    
    return true;
}

/**
 * @brief Process a line containing a shellcheck directive
 *
 * Processes a line of text that contains a shellcheck directive by extracting
 * the directive and adding it to the documentation block.
 *
 * @param docblock The documentation block to add the directive to
 * @param line The line of text containing a shellcheck directive
 *
 * @return bool true if the directive was successfully processed and added,
 *              false if an error occurred or if parameters are NULL
 *
 * @note This function first extracts the shellcheck directive using
 *       extract_shellcheck_directive(), then adds it to the docblock using
 *       add_shellcheck_directive()
 * @note The line must be a valid shellcheck directive (should be verified
 *       first with is_shellcheck_directive())
 * @see is_shellcheck_directive()
 * @see extract_shellcheck_directive()
 * @see add_shellcheck_directive()
 */
bool process_shellcheck_line(shellscribe_docblock_t *docblock, const char *line) {
    if (docblock == NULL || line == NULL) {
        return false;
    }
    if (!is_shellcheck_directive(line)) {
        return false;
    }
    char *directive = extract_shellcheck_directive(line);
    if (directive == NULL) {
        return false;
    }
    bool result = add_shellcheck_directive(docblock, directive);
    void *directive_ptr = directive;
    shell_free(&directive_ptr);
    
    return result;
} 
