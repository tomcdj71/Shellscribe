/**
 * @file argument.c
 * @brief Implementation of the argument parser
 *
 * This file contains functions for parsing and processing argument and parameter
 * tags in documentation blocks. It handles the extraction of argument names,
 * types, and descriptions from these tags and stores them in the appropriate
 * data structures for later rendering in the documentation.
 */

#include "parsers/argument.h"
#include "utils/string.h"
#include "utils/debug.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Check if a tag is an argument tag
 * 
 * Determines whether a given tag name indicates an argument or parameter
 * definition in the documentation. Recognizes 'arg', 'argument', and 'param'
 * as valid argument tag names.
 *
 * @param tag The tag name to check, without the '@' prefix
 * 
 * @return bool true if the tag is an argument tag, false otherwise or if tag is NULL
 * 
 * @note This function is case-sensitive, so tags must exactly match one of the
 *       recognized forms.
 */
bool is_argument_tag(const char *tag) {
    if (tag == NULL) {
        return false;
    }
    
    return (strcmp(tag, "arg") == 0 || strcmp(tag, "argument") == 0 || strcmp(tag, "param") == 0);
}

/**
 * @brief Process an argument tag (@arg)
 * 
 * Parses an argument tag's content and extracts the argument name, type,
 * and description. Adds the extracted information to the provided documentation
 * block for later rendering.
 *
 * The expected format is:
 * @arg name type description
 * 
 * Where:
 * - name: The argument name, typically a positional parameter like $1, $2
 * - type: The argument type (string, int, etc.), can be omitted
 * - description: A description of the argument's purpose and usage
 *
 * @param docblock The documentation block to add the argument to
 * @param content The content of the argument tag to process
 * 
 * @return bool true if argument was successfully processed and added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 *
 * @note The argument name is required, but type can be omitted.
 * @note Memory is allocated for each component of the argument, which will be
 *       freed when the docblock is freed.
 * @note If memory allocation fails at any point, any already-allocated memory
 *       for this argument is freed to prevent leaks.
 */
bool process_argument_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    const char *arg_str = content;
    char *arg_name = NULL;
    char *arg_type = NULL;
    char *arg_desc = NULL;
    while (isspace((unsigned char)*arg_str)) arg_str++;
    const char *name_end = arg_str;
    while (*name_end && !isspace((unsigned char)*name_end)) name_end++;
    if (name_end > arg_str) {
        arg_name = (char *)malloc(name_end - arg_str + 1);
        if (arg_name == NULL) return false;
        strncpy(arg_name, arg_str, name_end - arg_str);
        arg_name[name_end - arg_str] = '\0';
        arg_str = name_end;
    } else {
        return false;
    }
    while (isspace((unsigned char)*arg_str)) arg_str++;
    const char *type_end = arg_str;
    while (*type_end && !isspace((unsigned char)*type_end)) type_end++;
    if (type_end > arg_str) {
        arg_type = (char *)malloc(type_end - arg_str + 1);
        if (arg_type == NULL) {
            free(arg_name);
            return false;
        }
        strncpy(arg_type, arg_str, type_end - arg_str);
        arg_type[type_end - arg_str] = '\0';
        arg_str = type_end;
    }
    while (isspace((unsigned char)*arg_str)) arg_str++;
    arg_desc = string_duplicate(arg_str);
    shellscribe_argument_t *new_args = realloc(docblock->arguments, (docblock->arg_count + 1) * sizeof(shellscribe_argument_t));
    if (new_args == NULL) {
        free(arg_name);
        free(arg_type);
        free(arg_desc);
        return false;
    }
    docblock->arguments = new_args;
    docblock->arguments[docblock->arg_count].name = arg_name;
    docblock->arguments[docblock->arg_count].type = arg_type;
    docblock->arguments[docblock->arg_count].description = arg_desc;
    docblock->arg_count++;
    
    return true;
}

/**
 * @brief Process a parameter tag (@param)
 * 
 * Parses a parameter tag's content and extracts the parameter name and description.
 * Adds the extracted information to the provided documentation block for later
 * rendering. Unlike arguments, parameters do not include a type field.
 *
 * The expected format is:
 * @param name description
 * 
 * Where:
 * - name: The parameter name
 * - description: A description of the parameter's purpose and usage
 *
 * @param docblock The documentation block to add the parameter to
 * @param content The content of the parameter tag to process
 * 
 * @return bool true if parameter was successfully processed and added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 *
 * @note The parameter name is required.
 * @note Memory is allocated for each component of the parameter, which will be
 *       freed when the docblock is freed.
 * @note If memory allocation fails at any point, any already-allocated memory
 *       for this parameter is freed to prevent leaks.
 * @note This function is similar to process_argument_tag but with a simpler format
 *       that doesn't include a type field.
 * 
 * @see process_argument_tag
 */
bool process_parameter_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    const char *name_end = content;
    while (*name_end && !isspace((unsigned char)*name_end)) {
        name_end++;
    }
    if (name_end > content) {
        char *param_name = (char *)malloc(name_end - content + 1);
        if (param_name == NULL) {
            return false;
        }
        strncpy(param_name, content, name_end - content);
        param_name[name_end - content] = '\0';
        const char *desc_start = name_end;
        while (*desc_start && isspace((unsigned char)*desc_start)) {
            desc_start++;
        }
        void *new_params = realloc(docblock->params, (docblock->param_count + 1) * sizeof(*docblock->params));
        if (new_params == NULL) {
            free(param_name);
            return false;
        }
        docblock->params = new_params;
        docblock->params[docblock->param_count].name = param_name;
        docblock->params[docblock->param_count].description = string_duplicate(desc_start);
        docblock->param_count++;
        
        return true;
    }
    return false;
} 
