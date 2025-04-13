/**
 * @file option.c
 * @brief Implementation of option tag parser
 *
 * This file provides functionality for parsing and processing option tags
 * in documentation blocks. Options represent command-line arguments that
 * a script or tool accepts. The parser handles both short (-x) and long (--option)
 * option formats, extracts their arguments if present, and documents their purpose.
 */

#include "parsers/option.h"
#include "utils/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Check if a tag is an option tag
 * 
 * Determines whether a given tag name is related to command-line options.
 * Recognizes both 'option' and 'arg' as valid option-related tags.
 *
 * @param tag The tag name to check, without the '@' prefix
 * 
 * @return bool true if the tag is an option tag, false otherwise or if tag is NULL
 *
 * @note This function is case-sensitive, so tags must exactly match one of the
 *       recognized forms: "option" or "arg"
 * @note Although both tags are recognized, they might be processed slightly
 *       differently in some contexts
 */
bool is_option_tag(const char *tag) {
    if (tag == NULL) {
        return false;
    }
    
    return (strcmp(tag, "option") == 0 || strcmp(tag, "arg") == 0);
}

/**
 * @brief Parse the option content to extract the option and its description
 * 
 * Parses the content of an option tag and extracts the option name,
 * description, and argument specification (if present). Supports multiple
 * formats for option definitions including:
 * 
 * 1. "-o | option description" - option with pipe separator
 * 2. "-o description" - option with space separator
 * 3. "-o=<arg> description" - option with inline argument
 * 4. "-o <arg> description" - option with separate argument
 *
 * @param content The content of the option tag to parse
 * @param option Pointer to a string pointer that will store the extracted option
 * @param description Pointer to a string pointer that will store the extracted description
 * @param arg_spec Pointer to a string pointer that will store the extracted argument specification
 * 
 * @return bool true if parsing was successful and the option was extracted,
 *              false if an error occurred or if required parameters are NULL
 *
 * @note Memory is allocated for the option, description, and arg_spec (if present),
 *       which must be freed by the caller when no longer needed
 * @note If the option does not have an argument specification, arg_spec will be set to NULL
 * @note Argument specifications are typically enclosed in angle brackets (e.g., <file>)
 */
bool parse_option_content(const char *content, char **option, char **description, char **arg_spec) {
    if (content == NULL || option == NULL || description == NULL || arg_spec == NULL) {
        return false;
    }
    *option = NULL;
    *description = NULL;
    *arg_spec = NULL;
    while (isspace(*content)) {
        content++;
    }
    if (*content == '\0') {
        return false;
    }
    bool has_arg = false;
    const char *opt_start = content;
    const char *opt_end = NULL;
    const char *desc_start = NULL;
    const char *pipe = strstr(content, "|");
    if (pipe != NULL) {
        opt_end = pipe;
        while (opt_end > opt_start && isspace(*(opt_end - 1))) {
            opt_end--;
        }
        size_t opt_len = opt_end - opt_start;
        *option = malloc(opt_len + 1);
        if (*option == NULL) {
            return false;
        }
        strncpy(*option, opt_start, opt_len);
        (*option)[opt_len] = '\0';
        desc_start = pipe + 1;
        while (isspace(*desc_start)) {
            desc_start++;
        }
        const char *second_space = strchr(desc_start, ' ');
        if (second_space != NULL) {
            *description = string_duplicate(second_space + 1);
        } else {
            *description = string_duplicate("");
        }
        const char *arg_open = strchr(*option, '<');
        if (arg_open != NULL) {
            const char *arg_close = strchr(arg_open, '>');
            if (arg_close != NULL) {
                size_t arg_len = arg_close - arg_open - 1;
                *arg_spec = malloc(arg_len + 1);
                if (*arg_spec == NULL) {
                    free(*option);
                    free(*description);
                    return false;
                }
                
                strncpy(*arg_spec, arg_open + 1, arg_len);
                (*arg_spec)[arg_len] = '\0';
                has_arg = true;
            }
        }
        
        if (!has_arg) {
            arg_open = strchr(desc_start, '<');
            if (arg_open != NULL) {
                const char *arg_close = strchr(arg_open, '>');
                if (arg_close != NULL) {
                    size_t arg_len = arg_close - arg_open - 1;
                    *arg_spec = malloc(arg_len + 1);
                    if (*arg_spec == NULL) {
                        free(*option);
                        free(*description);
                        return false;
                    }
                    
                    strncpy(*arg_spec, arg_open + 1, arg_len);
                    (*arg_spec)[arg_len] = '\0';
                }
            }
        }
    } else {
        const char *space = strchr(content, ' ');
        if (space != NULL) {
            opt_end = space;
            desc_start = space + 1;
            size_t opt_len = opt_end - opt_start;
            *option = malloc(opt_len + 1);
            if (*option == NULL) {
                return false;
            }
            
            strncpy(*option, opt_start, opt_len);
            (*option)[opt_len] = '\0';
            *description = string_duplicate(desc_start);
        } else {
            *option = string_duplicate(content);
            *description = string_duplicate("");
        }
        const char *arg_open = NULL;
        const char *arg_close = NULL;
        const char *equals = strchr(*option, '=');
        if (equals != NULL) {
            arg_open = strchr(equals, '<');
            if (arg_open != NULL) {
                arg_close = strchr(arg_open, '>');
            }
        } else {
            arg_open = strchr(*option, '<');
            if (arg_open != NULL) {
                arg_close = strchr(arg_open, '>');
            }
        }
        if (arg_open != NULL && arg_close != NULL) {
            size_t arg_len = arg_close - arg_open - 1;
            *arg_spec = malloc(arg_len + 1);
            if (*arg_spec == NULL) {
                free(*option);
                free(*description);
                return false;
            }
            strncpy(*arg_spec, arg_open + 1, arg_len);
            (*arg_spec)[arg_len] = '\0';
        }
    }
    
    return true;
}

/**
 * @brief Process an option tag and add it to a documentation block
 * 
 * Parses an option tag's content, extracts the option name, description, and
 * argument specification, and adds this information to the provided documentation
 * block. Handles both short options (-x) and long options (--option).
 *
 * @param docblock The documentation block to add the option to
 * @param content The content of the option tag
 * 
 * @return bool true if the option was successfully processed and added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 *
 * @note This function uses parse_option_content to extract the option components
 * @note Memory is allocated for the option information, which will be freed
 *       when the docblock is freed
 * @note Only options starting with a dash (-) are recognized; others are rejected
 * @note Short options start with a single dash (e.g., -v) while long options
 *       start with two dashes (e.g., --verbose)
 * 
 * @see parse_option_content
 */
bool process_option_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    char *option = NULL;
    char *description = NULL;
    char *arg_spec = NULL;
    if (!parse_option_content(content, &option, &description, &arg_spec)) {
        return false;
    }
    char *short_opt = NULL;
    char *long_opt = NULL;
    
    if (option[0] == '-' && option[1] != '-') {
        short_opt = option;
    } else if (option[0] == '-' && option[1] == '-') {
        long_opt = option;
    } else {
        free(option);
        free(description);
        free(arg_spec);
        return false;
    }
    shellscribe_option_t *new_options = realloc(docblock->options, (docblock->option_count + 1) * sizeof(shellscribe_option_t));
    if (new_options == NULL) {
        free(option);
        free(description);
        free(arg_spec);
        return false;
    }
    
    docblock->options = new_options;
    docblock->options[docblock->option_count].short_opt = short_opt;
    docblock->options[docblock->option_count].long_opt = long_opt;
    docblock->options[docblock->option_count].arg_spec = arg_spec;
    docblock->options[docblock->option_count].description = description;
    docblock->option_count++;
        
    return true;
} 
