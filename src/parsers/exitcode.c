/**
 * @file exitcode.c
 * @brief Implementation of exitcode tag parser
 *
 * This file provides functionality for parsing and processing exitcode tags
 * in documentation blocks. Exit codes are important for command-line tools
 * to document the possible return values and their meanings, helping users
 * understand and handle potential error conditions.
 */

#include "parsers/exitcode.h"
#include "utils/string.h"
#include "utils/memory.h"
#include "utils/debug.h"
#include "utils/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Check if a tag is an exitcode tag
 * 
 * Determines whether a given tag name is an exitcode tag.
 * Exitcode tags document the possible return values of a shell function or script.
 *
 * @param tag The tag name to check, without the '@' prefix
 * 
 * @return bool true if the tag is an exitcode tag, false otherwise or if tag is NULL
 *
 * @note This function is case-sensitive, so the tag must exactly match "exitcode"
 */
bool is_exitcode_tag(const char *tag) {
    if (tag == NULL) {
        return false;
    }
    
    return (strcmp(tag, "exitcode") == 0);
}

/**
 * @brief Parse the exitcode content to extract the code and its description
 * 
 * Parses the content of an exitcode tag and extracts the exit code value
 * and its associated description. The expected format is:
 * "CODE description" where CODE is the numeric exit code and everything
 * after the first space is considered the description.
 *
 * @param content The content of the exitcode tag to parse
 * @param code Pointer to a string pointer that will be set to the extracted code
 * @param description Pointer to a string pointer that will be set to the extracted description
 * 
 * @return bool true if parsing was successful and the code was extracted,
 *              false if an error occurred or if required parameters are NULL
 *
 * @note Memory is allocated for both the code and description, which must be freed
 *       by the caller when no longer needed
 * @note If no space is found in the content, the entire content is treated as
 *       the code and the description is set to an empty string
 */
bool parse_exitcode_content(const char *content, char **code, char **description) {
    if (content == NULL || code == NULL || description == NULL) {
        return false;
    }
    
    *code = NULL;
    *description = NULL;
    while (isspace(*content)) {
        content++;
    }
    if (*content == '\0') {
        return false;
    }
    const char *code_start = content;
    const char *code_end = strchr(content, ' ');
    
    if (code_end == NULL) {
        *code = shell_strdup(content);
        *description = shell_strdup("");
        return true;
    }
    size_t code_len = code_end - code_start;
    *code = shell_malloc(code_len + 1);
    if (*code == NULL) {
        return false;
    }
    
    strncpy(*code, code_start, code_len);
    (*code)[code_len] = '\0';
    const char *desc_start = code_end + 1;
    while (isspace(*desc_start)) {
        desc_start++;
    }
    
    *description = shell_strdup(desc_start);
    
    return true;
}

/**
 * @brief Process an exitcode tag and add it to a documentation block
 * 
 * Parses an exitcode tag's content, extracts the code and description,
 * and adds this information to the provided documentation block.
 *
 * @param docblock The documentation block to add the exitcode to
 * @param content The content of the exitcode tag
 * 
 * @return bool true if the exitcode was successfully processed and added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 *
 * @note This function uses parse_exitcode_content to extract the code and description
 * @note Memory is allocated for the exitcode information, which will be freed
 *       when the docblock is freed
 * 
 * @see parse_exitcode_content
 */
bool process_exitcode_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    
    char *code = NULL;
    char *description = NULL;
    if (!parse_exitcode_content(content, &code, &description)) {
        return false;
    }
    shellscribe_exitcode_t *new_exitcodes = shell_realloc(docblock->exitcodes, (docblock->exitcode_count + 1) * sizeof(shellscribe_exitcode_t));
    if (new_exitcodes == NULL) {
        void *code_ptr = code;
        shell_free(&code_ptr);
        void *desc_ptr = description;
        shell_free(&desc_ptr);
        return false;
    }
    
    docblock->exitcodes = new_exitcodes;
    docblock->exitcodes[docblock->exitcode_count].code = code;
    docblock->exitcodes[docblock->exitcode_count].description = description;
    docblock->exitcode_count++;
    
    return true;
}

/**
 * @brief Parse an exit code tag with debugging support
 * 
 * Similar to process_exitcode_tag but includes additional debugging output
 * and uses a slightly different parsing approach. This function is an alternative
 * implementation that provides more verbose logging.
 *
 * @param docblock The documentation block to add the exitcode to
 * @param content The content of the exitcode tag
 * @param config Configuration settings that control debugging output
 * 
 * @return bool true if the exit code was successfully processed and added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 *
 * @note This function logs debug messages if debugging is enabled in the configuration
 * @note Memory is allocated for the exitcode information, which will be freed
 *       when the docblock is freed
 * 
 * @see process_exitcode_tag
 */
bool parse_exit_code_tag(shellscribe_docblock_t *docblock, const char *content, const shellscribe_config_t *config) {
    if (docblock == NULL || content == NULL || config == NULL) {
        return false;
    }
    
    debug_message(config, "Parsing exit code: '%s'\n", content);
    const char *code_end = content;
    while (*code_end && !isspace((unsigned char)*code_end)) {
        code_end++;
    }
    
    size_t code_len = code_end - content;
    if (code_len > 0) {
        char *code = shell_malloc(code_len + 1);
        if (code == NULL) {
            return false;
        }
        
        strncpy(code, content, code_len);
        code[code_len] = '\0';
        const char *description = code_end;
        while (*description && isspace((unsigned char)*description)) {
            description++;
        }
        shellscribe_exitcode_t *new_exitcodes = shell_realloc(docblock->exitcodes,(docblock->exitcode_count + 1) * sizeof(shellscribe_exitcode_t));
        if (new_exitcodes == NULL) {
            void *code_ptr = code;
            shell_free(&code_ptr);
            return false;
        }
        
        docblock->exitcodes = new_exitcodes;
        docblock->exitcodes[docblock->exitcode_count].code = code;
        docblock->exitcodes[docblock->exitcode_count].description = shell_strdup(description);
        docblock->exitcode_count++;
        debug_message(config, "Added exit code: code='%s', desc='%s'\n", code, description);
        
        return true;
    }
    
    return false;
} 
