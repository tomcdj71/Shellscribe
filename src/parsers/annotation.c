/**
 * @file annotation.c
 * @brief Implementation of special annotation parser for warnings, alerts, and other special tags
 *
 * This file provides functionality for parsing and processing various special
 * annotations and tag types that don't fit into other specific categories.
 * It handles warnings, dependencies, environment variables, and cross-reference
 * tags like "requires", "used-by", "calls", and "provides". These annotations
 * help document the dependencies and relationships between different functions.
 */

#include "parsers/annotation.h"
#include "parsers/types.h"
#include "utils/string.h"
#include "utils/debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAG_PREFIX "# @"

/**
 * @brief Check if a line is a comment
 *
 * Determines whether a given line is a comment line by checking if it starts
 * with a hash (#) character after any leading whitespace.
 *
 * @param line The line to check for a comment
 *
 * @return bool true if the line is a comment, false otherwise or if line is NULL
 */
static bool is_comment_line(const char *line) {
    if (line == NULL) {
        return false;
    }
    while (isspace((unsigned char)*line)) {
        line++;
    }
    return (line[0] == '#');
}

/**
 * @brief Check if a line is a tag line
 * 
 * Determines whether a given line contains a documentation tag.
 * Supports two formats for tags:
 * 1. Standard format: "# @tagname content"
 * 2. Alternative format: "# @tagname: content"
 *
 * @param line The line to check for a tag
 * 
 * @return bool true if the line contains a tag, false otherwise
 */
static bool is_tag_line(const char *line) {
    while (isspace((unsigned char)*line)) {
        line++;
    }
    if (strncmp(line, TAG_PREFIX, strlen(TAG_PREFIX)) == 0)
        return true;
    const char *at_sign = strstr(line, "@");
    if (at_sign != NULL && at_sign > line && *(at_sign-1) == '#') {
        const char *colon = strchr(at_sign, ':');
        if (colon != NULL)
            return true;
    }
    return false;
}

/**
 * @brief Extract tag name from a line
 *
 * Parses a line containing a documentation tag and extracts the tag name.
 * Supports both standard format ("# @tagname") and alternative format 
 * ("# @tagname:").
 *
 * @param line The line containing the tag
 *
 * @return char* Newly allocated string containing just the tag name,
 *               or NULL if the line does not contain a tag or if memory allocation fails.
 *               The caller is responsible for freeing this memory.
 */
static char *extract_tag_name(const char *line) {
    while (isspace((unsigned char)*line)) {
        line++;
    }
    if (strncmp(line, TAG_PREFIX, strlen(TAG_PREFIX)) == 0) {
        line += strlen(TAG_PREFIX);
        const char *end = line;
        while (*end && !isspace((unsigned char)*end) && *end != ':') {
            end++;
        }
        size_t len = end - line;
        char *tag = (char *)malloc(len + 1);
        if (tag == NULL) {
            return NULL;
        }
        strncpy(tag, line, len);
        tag[len] = '\0';
        
        return tag;
    } 
    else {
        const char *at_sign = strstr(line, "@");
        if (at_sign != NULL) {
            const char *tag_start = at_sign + 1;
            const char *end = tag_start;
            while (*end && !isspace((unsigned char)*end) && *end != ':') {
                end++;
            }
            size_t len = end - tag_start;
            char *tag = (char *)malloc(len + 1);
            if (tag == NULL) {
                return NULL;
            }
            strncpy(tag, tag_start, len);
            tag[len] = '\0';
            return tag;
        }
    }
    return NULL;
}

/**
 * @brief Extract tag content from a line
 * 
 * Parses a line containing a documentation tag and extracts the content
 * (everything after the tag name and any whitespace or colon).
 * Supports both standard format ("# @tagname content") and alternative
 * format ("# @tagname: content").
 * 
 * @param line The line containing the tag and its content
 * 
 * @return char* Newly allocated string containing just the tag content,
 *               or NULL if the line does not contain a tag or if memory allocation fails.
 *               The caller is responsible for freeing this memory.
 */
static char *extract_tag_content(const char *line) {
    while (isspace((unsigned char)*line)) {
        line++;
    }
    if (strncmp(line, TAG_PREFIX, strlen(TAG_PREFIX)) == 0) {
        line += strlen(TAG_PREFIX);
        while (*line && !isspace((unsigned char)*line)) {
            line++;
        }
        while (isspace((unsigned char)*line)) {
            line++;
        }
        return string_duplicate(line);
    } else {
        const char *at_sign = strstr(line, "@");
        if (at_sign != NULL) {
            const char *colon = strchr(at_sign, ':');
            if (colon != NULL) {
                const char *content_start = colon + 1;
                while (isspace((unsigned char)*content_start)) {
                    content_start++;
                }
                return string_duplicate(content_start);
            } else {
                const char *tag_start = at_sign + 1;
                const char *space = tag_start;
                while (*space && !isspace((unsigned char)*space)) {
                    space++;
                }
                if (*space) {
                    const char *content_start = space;
                    while (isspace((unsigned char)*content_start)) {
                        content_start++;
                    }
                    return string_duplicate(content_start);
                }
            }
        }
    }
    
    return NULL;
}

/**
 * @brief Check if a line contains a special annotation
 * 
 * Determines whether a given line contains a special annotation like
 * shellcheck directives or TODO/FIXME comments.
 * 
 * @param line The line to check for special annotations
 * 
 * @return bool true if the line contains a special annotation, false otherwise or if line is NULL
 */
bool is_special_annotation(const char *line) {
    if (line == NULL) {
        return false;
    }
    static const char *keywords[] = {
        "shellcheck", "disable", "TODO", "FIXME", "XXX", "HACK"
    };
    size_t keyword_count = sizeof(keywords) / sizeof(keywords[0]);
    for (size_t i = 0; i < keyword_count; i++) {
        if (strstr(line, keywords[i]) != NULL) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Process a special annotation in a line
 * 
 * Placeholder function for processing special annotations that might
 * be present in comment lines.
 * 
 * @param line The line containing the special annotation
 * @param data Pointer to data that might be used for processing the annotation
 * 
 * @return bool true if the annotation was successfully processed,
 *              false otherwise or if line is NULL
 */
bool process_special_annotation(const char *line, void *data) {
    if (line == NULL) {
        return false;
    }
    return true;
}

/**
 * @brief Process an alert tag
 * 
 * Internal implementation for processing an alert tag and storing it
 * in a documentation block.
 * 
 * @param docblock The documentation block to add the alert to
 * @param type The type of the alert (e.g., "NOTE", "WARNING")
 * @param content The content/message of the alert
 * 
 * @return bool true if the alert was successfully processed and added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 * 
 * @note Memory is allocated for the alert type and content, which will be
 *       freed when the docblock is freed.
 */
static bool process_alert_tag_internal(shellscribe_docblock_t *docblock, const char *type, const char *content) {
    if (docblock == NULL || type == NULL || content == NULL) {
        return false;
    }
    shellscribe_alert_t *new_alerts = realloc(docblock->alerts, (docblock->alert_count + 1) * sizeof(shellscribe_alert_t));
    if (new_alerts == NULL) {
        return false;
    }
    docblock->alerts = new_alerts;
    docblock->alerts[docblock->alert_count].type = string_duplicate(type);
    docblock->alerts[docblock->alert_count].content = string_duplicate(content);
    docblock->alert_count++;
    
    return true;
}

/**
 * @brief Process a warning tag (@warning)
 * 
 * Adds a warning message to a documentation block.
 * Warnings highlight potential issues or pitfalls when using the function.
 * 
 * @param docblock The documentation block to add the warning to
 * @param content The warning message content
 * 
 * @return bool true if the warning was successfully processed and added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 * 
 * @note Memory is allocated for the warning string, which will be freed
 *       when the docblock is freed.
 */
bool process_warning_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    char **new_warnings = realloc(docblock->warnings, (docblock->warning_count + 1) * sizeof(char *));
    if (new_warnings == NULL) {
        return false;
    }
    docblock->warnings = new_warnings;
    docblock->warnings[docblock->warning_count] = string_duplicate(content);
    docblock->warning_count++;
    
    return true;
}

/**
 * @brief Process a dependency tag (@dependency)
 * 
 * Adds a general dependency to a documentation block.
 * Dependencies represent things that the function relies on to work properly.
 * 
 * @param docblock The documentation block to add the dependency to
 * @param content The dependency name or description
 * 
 * @return bool true if the dependency was successfully processed and added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 * 
 * @note Memory is allocated for the dependency string, which will be freed
 *       when the docblock is freed.
 */
bool process_dependency_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    char **new_dependencies = realloc(docblock->dependencies, (docblock->dependency_count + 1) * sizeof(char *));
    if (new_dependencies == NULL) {
        return false;
    }
    
    docblock->dependencies = new_dependencies;
    docblock->dependencies[docblock->dependency_count] = string_duplicate(content);
    docblock->dependency_count++;
    
    return true;
}

/**
 * @brief Process an internal call tag (@internal_call)
 * 
 * Records an internal function call made by the documented function.
 * This helps document the internal dependency graph of the application.
 * 
 * @param docblock The documentation block to add the internal call to
 * @param content The name of the internal function being called
 * 
 * @return bool true if the internal call was successfully processed and added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 * 
 * @note Memory is allocated for the internal call string, which will be freed
 *       when the docblock is freed.
 */
bool process_internal_call_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    char **new_internal_calls = realloc(docblock->internal_calls, (docblock->internal_call_count + 1) * sizeof(char *));
    if (new_internal_calls == NULL) {
        return false;
    }
    
    docblock->internal_calls = new_internal_calls;
    docblock->internal_calls[docblock->internal_call_count] = string_duplicate(content);
    docblock->internal_call_count++;
    
    return true;
}

/**
 * @brief Process an environment variable tag (@env)
 * 
 * Records an environment variable that affects the function's behavior.
 * The tag content should include the variable name followed by its description.
 * 
 * @param docblock The documentation block to add the environment variable to
 * @param content The environment variable information (name and description)
 * 
 * @return bool true if the environment variable was successfully processed and added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 * 
 * @note The content format should be "NAME description" where NAME is the
 *       environment variable name and everything after it is the description
 * @note Memory is allocated for the environment variable information, which will be freed
 *       when the docblock is freed.
 */
bool process_environment_var_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    const char *name_end = content;
    while (*name_end && !isspace((unsigned char)*name_end)) {
        name_end++;
    }
    
    if (name_end == content) {
        return false;
    }
    char *name = (char *)malloc(name_end - content + 1);
    if (name == NULL) {
        return false;
    }
    
    strncpy(name, content, name_end - content);
    name[name_end - content] = '\0';
    const char *description = name_end;
    while (*description && isspace((unsigned char)*description)) {
        description++;
    }
    shellscribe_env_var_t *new_env_vars = realloc(docblock->env_vars, (docblock->env_var_count + 1) * sizeof(shellscribe_env_var_t));
    if (new_env_vars == NULL) {
        free(name);
        return false;
    }
    
    docblock->env_vars = new_env_vars;
    docblock->env_vars[docblock->env_var_count].name = name;
    docblock->env_vars[docblock->env_var_count].description = string_duplicate(description);
    docblock->env_vars[docblock->env_var_count].default_value = NULL;
    docblock->env_var_count++;
    
    return true;
}

/**
 * @brief Process a requires tag (@requires)
 * 
 * Records a required dependency for the documented function.
 * These are typically external tools, libraries, or other components
 * that must be available for the function to work correctly.
 * 
 * @param docblock The documentation block to add the requirement to
 * @param content The name of the required dependency
 * 
 * @return bool true if the requirement was successfully processed and added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 * 
 * @note Memory is allocated for the requirement string, which will be freed
 *       when the docblock is freed.
 */
bool process_requires_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    char **new_requires = realloc(docblock->requires, (docblock->requires_count + 1) * sizeof(char *));
    if (new_requires == NULL) {
        return false;
    }
    
    docblock->requires = new_requires;
    docblock->requires[docblock->requires_count] = string_duplicate(content);
    docblock->requires_count++;
    
    return true;
}

/**
 * @brief Process a used-by tag (@used-by)
 * 
 * Records functions that use the documented function.
 * This helps document the reverse dependency graph of the application.
 * 
 * @param docblock The documentation block to add the used-by information to
 * @param content The name of the function that uses this function
 * 
 * @return bool true if the used-by information was successfully processed and added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 * 
 * @note Memory is allocated for the used-by string, which will be freed
 *       when the docblock is freed.
 */
bool process_used_by_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    char **new_used_by = realloc(docblock->used_by, (docblock->used_by_count + 1) * sizeof(char *));
    if (new_used_by == NULL) {
        return false;
    }
    
    docblock->used_by = new_used_by;
    docblock->used_by[docblock->used_by_count] = string_duplicate(content);
    docblock->used_by_count++;
    
    return true;
}

/**
 * @brief Process an external call tag (@calls)
 * 
 * Records external functions, commands, or tools that are called by the documented function.
 * This helps document the external dependencies of the function.
 * 
 * @param docblock The documentation block to add the external call information to
 * @param content The name of the external function or command that is called
 * 
 * @return bool true if the external call was successfully processed and added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 * 
 * @note Memory is allocated for the external call string, which will be freed
 *       when the docblock is freed.
 */
bool process_calls_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    char **new_calls = realloc(docblock->calls, (docblock->calls_count + 1) * sizeof(char *));
    if (new_calls == NULL) {
        return false;
    }
    
    docblock->calls = new_calls;
    docblock->calls[docblock->calls_count] = string_duplicate(content);
    docblock->calls_count++;
    
    return true;
}

/**
 * @brief Process a provides tag (@provides)
 * 
 * Records services or features provided by the documented function.
 * This helps document the capabilities and responsibilities of the function.
 * 
 * @param docblock The documentation block to add the provides information to
 * @param content Description of the service or feature provided
 * 
 * @return bool true if the provides information was successfully processed and added to the docblock,
 *              false if an error occurred or if required parameters are NULL
 * 
 * @note Memory is allocated for the provides string, which will be freed
 *       when the docblock is freed.
 */
bool process_provides_tag(shellscribe_docblock_t *docblock, const char *content) {
    if (docblock == NULL || content == NULL) {
        return false;
    }
    char **new_provides = realloc(docblock->provides, (docblock->provides_count + 1) * sizeof(char *));
    if (new_provides == NULL) {
        return false;
    }
    
    docblock->provides = new_provides;
    docblock->provides[docblock->provides_count] = string_duplicate(content);
    docblock->provides_count++;
    
    return true;
} 
