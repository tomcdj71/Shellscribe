/**
 * @file function.c
 * @brief Implementation of function parsing utilities
 *
 * This file provides functionality for identifying and parsing shell function
 * declarations in source code. It handles the extraction of function names
 * from declaration lines and updates documentation blocks with the parsed
 * function information.
 */

#include "parsers/function.h"
#include "parsers/types.h"
#include "utils/string.h"
#include "utils/debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Check if a line is a function declaration
 * 
 * Examines a line of code to determine if it contains a shell function declaration.
 * Recognizes both the "function name() {" syntax and the simpler "name() {" syntax
 * that is commonly used in shell scripts.
 *
 * @param line The line to check for a function declaration
 * 
 * @return bool true if the line is a function declaration, false otherwise or if line is NULL
 * 
 * @note A function declaration is recognized by the following patterns:
 *       1. "function name() {" - with the 'function' keyword
 *       2. "name() {" - without the 'function' keyword
 * @note Function name validation ensures that the name consists of only alphanumeric
 *       characters and underscores
 * @note Whitespace between the closing parenthesis and opening brace is allowed
 */
bool is_function_declaration(const char *line) {
    if (line == NULL) {
        return false;
    }
    const char *pos = line;
    while (isspace((unsigned char)*pos)) {
        pos++;
    }
    if (strncmp(pos, "function", 8) == 0 && isspace((unsigned char)pos[8])) {
        return true;
    }
    const char *parenthesis = strchr(pos, '(');
    if (parenthesis && parenthesis > pos) {
        for (const char *p = pos; p < parenthesis; p++) {
            if (!isalnum((unsigned char)*p) && *p != '_') {
                return false;
            }
        }
        const char *closing = strchr(parenthesis, ')');
        if (closing) {
            const char *brace = strchr(closing, '{');
            if (brace) {
                for (const char *p = closing + 1; p < brace; p++) {
                    if (!isspace((unsigned char)*p)) {
                        return false;
                    }
                }
                return true;
            }
        }
    }
    
    return false;
}

/**
 * @brief Extract the function name from a function declaration line
 * 
 * Parses a function declaration line and extracts just the function name.
 * Handles both function declaration styles (with and without the 'function' keyword).
 *
 * @param line The line containing the function declaration
 * 
 * @return char* Newly allocated string containing just the function name,
 *               or NULL if the line is not a function declaration, is NULL,
 *               or if memory allocation fails. The caller is responsible for
 *               freeing this memory.
 * 
 * @note Function name is extracted up to the opening parenthesis '('
 * @note Trailing whitespace in the function name is trimmed
 * @note If the 'function' keyword is present, it is skipped
 * 
 * @see is_function_declaration
 */
char *extract_function_name(const char *line) {
    if (line == NULL || !is_function_declaration(line)) {
        return NULL;
    }
    const char *pos = line;
    while (isspace((unsigned char)*pos)) {
        pos++;
    }
    if (strncmp(pos, "function", 8) == 0 && isspace((unsigned char)pos[8])) {
        pos += 8;
        while (isspace((unsigned char)*pos)) {
            pos++;
        }
    }
    
    const char *end = strchr(pos, '(');
    if (end && end > pos) {
        size_t len = end - pos;
        char *func_name = (char *)malloc(len + 1);
        if (func_name) {
            strncpy(func_name, pos, len);
            func_name[len] = '\0';
            len = strlen(func_name);
            while (len > 0 && isspace((unsigned char)func_name[len - 1])) {
                func_name[--len] = '\0';
            }
        }
        return func_name;
    }
    
    return NULL;
}

/**
 * @brief Parse a function declaration line and update the docblock
 * 
 * Extracts the function name from a declaration line and updates the
 * provided documentation block with this information. This associates
 * the preceding docblock comments with the function being defined.
 *
 * @param docblock The documentation block to update with the function name
 * @param line The line containing the function declaration
 * 
 * @return bool true if the function declaration was successfully parsed and
 *              the docblock was updated, false if an error occurred or if
 *              required parameters are NULL
 * 
 * @note Any existing function name in the docblock will be freed and replaced
 * @note Memory is allocated for the function name, which will be freed when
 *       the docblock is freed
 * 
 * @see extract_function_name
 */
bool parse_function_declaration(shellscribe_docblock_t *docblock, const char *line) {
    if (docblock == NULL || line == NULL) {
        return false;
    }
    char *function_name = extract_function_name(line);
    if (function_name == NULL) {
        return false;
    }
    free(docblock->function_name);
    docblock->function_name = function_name;
    
    return true;
} 

