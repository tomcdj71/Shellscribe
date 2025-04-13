/**
 * @file function.h
 * @brief Function parsing utilities for shell script documentation
 */

#ifndef SHELLSCRIBE_FUNCTION_H
#define SHELLSCRIBE_FUNCTION_H

#include <stdbool.h>

/**
 * @brief Check if a line is a function declaration
 *
 * @param line The line to check
 * @return true if the line is a function declaration, false otherwise
 */
bool is_function_declaration(const char *line);

/**
 * @brief Extract the function name from a function declaration line
 *
 * @param line The line containing a function declaration
 * @return The function name, or NULL if no function was found
 */
char *extract_function_name(const char *line);

#endif /* SHELLSCRIBE_FUNCTION_H */
