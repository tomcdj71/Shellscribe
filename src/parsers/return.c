/**
 * @file return.c
 * @brief Implementation of return tag parser
 *
 * This file provides functionality for parsing and processing return-related tags
 * in documentation blocks. Return tags document the values that a function can
 * return and their meanings. For shell scripts, this can include both exit codes
 * for commands and return values for functions that output results.
 */

#include "parsers/return.h"
#include "parsers/state.h"
#include "utils/string.h"
#include "utils/memory.h"
#include "utils/debug.h"
#include "utils/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Check if a tag is a return-related tag
 * 
 * Determines whether a given tag name is related to function return values.
 * Recognizes both 'return' and 'returns' as valid return-related tags.
 *
 * @param tag The tag name to check, without the '@' prefix
 * 
 * @return bool true if the tag is a return-related tag, false otherwise or if tag is NULL
 *
 * @note This function is case-sensitive, so tags must exactly match one of the
 *       recognized forms: "return" or "returns"
 * @note Both "return" and "returns" tags are treated identically, with "returns"
 *       being an alias for "return"
 */
bool is_return_tag(const char *tag) {
    if (tag == NULL) {
        return false;
    }
    
    return (strcmp(tag, "return") == 0 || strcmp(tag, "returns") == 0);
}

/**
 * @brief Process a return value tag (@return)
 * 
 * Stores documentation about what a function returns.
 * This helps users understand the output format, possible return values,
 * and their meanings.
 *
 * @param docblock The documentation block to add the return value documentation to
 * @param content Description of what the function returns
 * 
 * @return bool true if the return value information was successfully processed and
 *              added to the docblock, false if an error occurred or if
 *              required parameters are NULL
 *
 * @note Any existing return value documentation in the docblock will be freed
 *       before the new content is assigned
 * @note Memory is allocated for the return value documentation, which will be freed
 *       when the docblock is freed
 */
bool process_return_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    free(docblock->return_desc);
    docblock->return_desc = string_duplicate(content);
    
    return (docblock->return_desc != NULL);
}

/**
 * @brief Process a returns value tag (@returns) - alias for @return
 * 
 * This is an alias for process_return_tag that handles the @returns tag.
 * Both @return and @returns tags are semantically equivalent and serve the
 * same purpose in the documentation.
 *
 * @param docblock The documentation block to add the return value documentation to
 * @param content Description of what the function returns
 * 
 * @return bool true if the return value information was successfully processed,
 *              false if an error occurred
 *
 * @note This is a direct wrapper around process_return_tag
 * 
 * @see process_return_tag
 */
bool process_returns_tag(shellscribe_docblock_t *docblock, const char *content) {
    return process_return_tag(docblock, content);
}

/**
 * @brief Parses an exit code tag content (@exitcode)
 * 
 * Processes an exit code tag to extract the numeric exit code value and its description.
 * Exit codes are especially important for command-line tools and shell scripts
 * to document possible termination statuses.
 *
 * @param docblock The documentation block to add the exit code information to
 * @param content The content of the exit code tag to parse
 * @param config Configuration settings that control debugging output
 * 
 * @return bool true if the exit code was successfully processed and added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 *
 * @note The expected format is "CODE description" where CODE is the numeric exit code
 *       and everything after the first space is considered the description
 * @note Memory is allocated for the exit code information, which will be freed
 *       when the docblock is freed
 * @note This function logs debug messages if debugging is enabled in the configuration
 * @note Exit codes are stored in the returns array of the docblock structure
 */
bool parse_exit_code(shellscribe_docblock_t *docblock, const char *content, const shellscribe_config_t *config) {
    if (docblock == NULL || content == NULL || config == NULL) {
        return false;
    }
    
    debug_message(config, "Parsing exit code: '%s'\n", content);
    const char *value_end = content;
    while (*value_end && !isspace((unsigned char)*value_end)) {
        value_end++;
    }
    if (value_end > content) {
        char *value = (char *)shell_malloc(value_end - content + 1);
        if (value == NULL) {
            return false;
        }
        strncpy(value, content, value_end - content);
        value[value_end - content] = '\0';
        const char *description = value_end;
        while (*description && isspace((unsigned char)*description)) {
            description++;
        }
        void *new_returns = shell_realloc(docblock->returns, (docblock->return_count + 1) * sizeof(shellscribe_return_t));
        if (new_returns == NULL) {
            void *value_ptr = value;
            shell_free(&value_ptr);
            return false;
        }
        
        docblock->returns = new_returns;
        ((shellscribe_return_t*)docblock->returns)[docblock->return_count].value = value;
        ((shellscribe_return_t*)docblock->returns)[docblock->return_count].description = string_duplicate(description);
        docblock->return_count++;
        debug_message(config, "Added exit code: value='%s', desc='%s'\n", value, description);
        
        return true;
    }
    return false;
} 
