/**
* @file debug.h
* @brief Debugging functions for shellscribe
*/

#ifndef SHELLSCRIBE_DEBUG_H
#define SHELLSCRIBE_DEBUG_H

#include <stdio.h>
#include <stdarg.h>
#include "utils/config.h"

/**
* @brief Writes a debug message to the error output stream
*
* @param config Pointer to the configuration
* @param format Message format (like printf)
* @param ... Format arguments
*/
void debug_message(const shellscribe_config_t *config, const char *format, ...);

/**
* @brief Writes an error message and exits the program
*
* @param line_number Line number where the error occurred
* @param message Error message to display
*/
void error_message(int line_number, const char *message);

/**
* @brief Writes a warning message
*
* @param line_number Line number where the warning occurred
* @param message Warning message to display
*/
void warning_message(int line_number, const char *message);

#endif /* SHELLSCRIBE_DEBUG_H */
