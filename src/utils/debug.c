/**
 * @file debug.c
 * @brief Debugging utility functions
 */

#include "utils/debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/**
 * @brief Outputs a debug message to stderr if debugging is enabled
 *
 * @param config The configuration structure containing debug settings
 * @param format The format string for the debug message
 * @param ... Variable arguments to be formatted according to the format string
 */
void debug_message(const shellscribe_config_t *config, const char *format, ...) {
    if (config == NULL || !config->debug) {
        return;
    }
    va_list args;
    va_start(args, format);
    fprintf(stderr, "[DEBUG] ");
    vfprintf(stderr, format, args);
    va_end(args);
}

/**
 * @brief Outputs an error message to stderr and exits the program
 *
 * @param line_number The line number where the error occurred
 * @param message The error message to display
 */
void error_message(int line_number, const char *message) {
    const char *error_color = "\033[1;31m";
    const char *color_reset = "\033[0m";
    fprintf(stderr, "%sERROR line %d: %s%s\n", error_color, line_number, message, color_reset);
    exit(1);
}

/**
 * @brief Outputs a warning message to stderr
 *
 * @param line_number The line number where the warning occurred
 * @param message The warning message to display
 */
void warning_message(int line_number, const char *message) {
    const char *warn_color = "\033[1;34m";
    const char *color_reset = "\033[0m";
    fprintf(stderr, "%sWARNING line %d: %s%s\n", warn_color, line_number, message, color_reset);
}
