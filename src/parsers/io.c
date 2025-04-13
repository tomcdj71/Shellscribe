/**
 * @file io.c
 * @brief Implementation of IO-related tags parser (@stdin, @stdout, @stderr)
 *
 * This file provides functionality for parsing and processing IO-related tags
 * in documentation blocks. These tags document how a function interacts with
 * standard input, output, and error streams, which is particularly important
 * for shell scripts that often process text streams.
 */

#include "parsers/io.h"
#include "utils/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Check if a tag is an IO-related tag
 * 
 * Determines whether a given tag name is related to standard IO streams.
 * Recognizes 'stdin', 'stdout', and 'stderr' as valid IO-related tags.
 *
 * @param tag The tag name to check, without the '@' prefix
 * 
 * @return bool true if the tag is an IO-related tag, false otherwise or if tag is NULL
 *
 * @note This function is case-sensitive, so tags must exactly match one of the
 *       recognized forms: "stdin", "stdout", or "stderr"
 * @note These tags are typically used to document how a function interacts with
 *       the standard input/output/error streams
 */
bool is_io_tag(const char *tag) {
    if (tag == NULL) {
        return false;
    }
    
    return (strcmp(tag, "stdin") == 0 || strcmp(tag, "stdout") == 0 || strcmp(tag, "stderr") == 0);
}

/**
 * @brief Process a stdin tag (@stdin)
 * 
 * Stores documentation about how a function uses standard input.
 * This is important for command-line tools that read from stdin.
 *
 * @param docblock The documentation block to add the stdin documentation to
 * @param content Description of how the function uses standard input
 * 
 * @return bool true if the stdin information was successfully processed and
 *              added to the docblock, false if an error occurred or if
 *              required parameters are NULL
 *
 * @note Any existing stdin documentation in the docblock will be freed
 *       before the new content is assigned
 * @note Memory is allocated for the stdin documentation, which will be freed
 *       when the docblock is freed
 */
bool process_stdin_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    free(docblock->stdin_doc);
    docblock->stdin_doc = string_duplicate(content);
    
    return (docblock->stdin_doc != NULL);
}

/**
 * @brief Process a stdout tag (@stdout)
 * 
 * Stores documentation about what a function writes to standard output.
 * This helps users understand the format and meaning of the function's output.
 *
 * @param docblock The documentation block to add the stdout documentation to
 * @param content Description of what the function outputs to stdout
 * 
 * @return bool true if the stdout information was successfully processed and
 *              added to the docblock, false if an error occurred or if
 *              required parameters are NULL
 *
 * @note Any existing stdout documentation in the docblock will be freed
 *       before the new content is assigned
 * @note Memory is allocated for the stdout documentation, which will be freed
 *       when the docblock is freed
 */
bool process_stdout_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    free(docblock->stdout_doc);
    docblock->stdout_doc = string_duplicate(content);
    
    return (docblock->stdout_doc != NULL);
}

/**
 * @brief Process a stderr tag (@stderr)
 * 
 * Stores documentation about what a function writes to standard error.
 * This helps users understand the error messages and warnings that might
 * be generated during execution.
 *
 * @param docblock The documentation block to add the stderr documentation to
 * @param content Description of what the function outputs to stderr
 * 
 * @return bool true if the stderr information was successfully processed and
 *              added to the docblock, false if an error occurred or if
 *              required parameters are NULL
 *
 * @note Any existing stderr documentation in the docblock will be freed
 *       before the new content is assigned
 * @note Memory is allocated for the stderr documentation, which will be freed
 *       when the docblock is freed
 */
bool process_stderr_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    free(docblock->stderr_doc);
    docblock->stderr_doc = string_duplicate(content);
    
    return (docblock->stderr_doc != NULL);
} 
