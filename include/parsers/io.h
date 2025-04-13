/**
 * @file io.h
 * @brief Parser for IO-related tags (@stdin, @stdout, @stderr) in shell script documentation
 */

#ifndef SHELLSCRIBE_IO_H
#define SHELLSCRIBE_IO_H

#include "parsers/types.h"
#include <stdbool.h>

/**
 * @brief Process a stdin tag
 *
 * @param docblock The documentation block
 * @param content The tag content
 * @return true if successful, false otherwise
 */
bool process_stdin_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Process a stdout tag
 *
 * @param docblock The documentation block
 * @param content The tag content
 * @return true if successful, false otherwise
 */
bool process_stdout_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Process a stderr tag
 *
 * @param docblock The documentation block
 * @param content The tag content
 * @return true if successful, false otherwise
 */
bool process_stderr_tag(shellscribe_docblock_t *docblock, const char *content);

/**
 * @brief Check if a tag is an IO-related tag
 *
 * @param tag The tag name
 * @return true if the tag is an IO-related tag, false otherwise
 */
bool is_io_tag(const char *tag);

#endif /* SHELLSCRIBE_IO_H */ 
