/**
 * @file types.c
 * @brief Implementation of common types and structures for the parsers
 *
 * This file provides utility functions for working with the core data structures
 * used throughout the parser system. It includes functions for initializing,
 * managing, and freeing documentation block structures and their components.
 * These functions are essential for proper memory management and initialization
 * of the data structures that store parsed documentation.
 */

#include "parsers/types.h"
#include "utils/memory.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Initialize a documentation block with default values
 * 
 * This function initializes a shellscribe_docblock_t structure with default values,
 * setting all pointers to NULL and counters to zero. It prepares the structure for
 * use in parsing documentation blocks.
 *
 * @param docblock Pointer to the documentation block structure to initialize
 *
 * @note If docblock is NULL, the function returns without doing anything
 * @note All string fields are set to NULL
 * @note All counters and flags are set to their default values (0 or false)
 * @note This function must be called before using a documentation block structure
 */
void init_docblock(shellscribe_docblock_t *docblock) {
    if (docblock == NULL) {
        return;
    }
    void **pointers[] = {
        (void **)&docblock->file_name,
        (void **)&docblock->brief,
        (void **)&docblock->description,
        (void **)&docblock->version,
        (void **)&docblock->author,
        (void **)&docblock->author_contact,
        (void **)&docblock->project,
        (void **)&docblock->license,
        (void **)&docblock->copyright,
        (void **)&docblock->interpreter,
        (void **)&docblock->function_name,
        (void **)&docblock->function_description,
        (void **)&docblock->function_brief,
        (void **)&docblock->alias,
        (void **)&docblock->return_desc,
        (void **)&docblock->section,
        (void **)&docblock->arguments,
        (void **)&docblock->params,
        (void **)&docblock->returns,
        (void **)&docblock->stdin_doc,
        (void **)&docblock->stdout_doc,
        (void **)&docblock->stderr_doc,
        (void **)&docblock->exitcodes,
        (void **)&docblock->options,
        (void **)&docblock->env_vars,
        (void **)&docblock->example,
        (void **)&docblock->see_also,
        (void **)&docblock->deprecation.version,
        (void **)&docblock->deprecation.replacement,
        (void **)&docblock->deprecation.eol,
        (void **)&docblock->alerts,
        (void **)&docblock->warnings,
        (void **)&docblock->dependencies,
        (void **)&docblock->internal_calls,
        (void **)&docblock->requires,
        (void **)&docblock->used_by,
        (void **)&docblock->calls,
        (void **)&docblock->provides,
        (void **)&docblock->set_vars,
        (void **)&docblock->shellcheck_directives
    };
    for (size_t i = 0; i < sizeof(pointers) / sizeof(pointers[0]); i++) {
        *pointers[i] = NULL;
    }
    *docblock = (shellscribe_docblock_t){
        .arg_count = 0,
        .no_args = false,
        .param_count = 0,
        .return_count = 0,
        .exitcode_count = 0,
        .option_count = 0,
        .env_var_count = 0,
        .see_also_count = 0,
        .is_internal = false,
        .is_skipped = false,
        .deprecation.is_deprecated = false,
        .alert_count = 0,
        .warning_count = 0,
        .dependency_count = 0,
        .internal_call_count = 0,
        .requires_count = 0,
        .used_by_count = 0,
        .calls_count = 0,
        .provides_count = 0,
        .set_var_count = 0,
        .shellcheck_count = 0
    };
}
#define FREE_FIELD(f) do {        \
    void *ptr = (f);              \
    shell_free(&ptr);             \
    (f) = NULL;                   \
} while (0)
#define FREE_ARRAY(arr, count, free_func) do {    \
    for (int i = 0; i < (count); ++i) {           \
        free_func(&(arr)[i]);                     \
    }                                             \
    shell_free((void**)&(arr));                   \
    (arr) = NULL;                                 \
    (count) = 0;                                  \
} while (0)
#define FREE_STRING_FIELDS_3(f1, f2, f3) \
    FREE_FIELD(f1);                      \
    FREE_FIELD(f2);                      \
    FREE_FIELD(f3)
#define FREE_STRING_FIELDS_2(f1, f2) \
    FREE_FIELD(f1);                  \
    FREE_FIELD(f2)

#define FREE_STRING_FIELDS_4(f1, f2, f3, f4) \
    FREE_FIELD(f1);                          \
    FREE_FIELD(f2);                          \
    FREE_FIELD(f3);                          \
    FREE_FIELD(f4)

static void free_argument(shellscribe_argument_t *arg) {
    FREE_STRING_FIELDS_3(arg->name, arg->type, arg->description);
}

static void free_param(shellscribe_param_t *param) {
    FREE_STRING_FIELDS_2(param->name, param->description);
}

static void free_return(shellscribe_return_t *ret) {
    FREE_STRING_FIELDS_2(ret->value, ret->description);
}

static void free_option(shellscribe_option_t *opt) {
    FREE_STRING_FIELDS_4(opt->short_opt, opt->long_opt, opt->arg_spec, opt->description);
}

static void free_env(shellscribe_env_var_t *env) {
    FREE_STRING_FIELDS_3(env->name, env->default_value, env->description);
}

static void free_seealso(shellscribe_see_also_t *ref) {
    FREE_STRING_FIELDS_2(ref->name, ref->url);
}

static void free_alert(shellscribe_alert_t *alert) {
    FREE_STRING_FIELDS_2(alert->type, alert->content);
}

static void free_setvar(shellscribe_global_var_t *var) {
    FREE_STRING_FIELDS_4(var->name, var->type, var->default_value, var->description);
}

static void free_shellcheck(shellscribe_shellcheck_t *shellcheck) {
    FREE_STRING_FIELDS_3(shellcheck->code, shellcheck->directive, shellcheck->reason);
}

/**
 * @brief Free all resources used by a documentation block
 *
 * This function frees all dynamically allocated memory associated with a
 * documentation block structure. It recursively frees all strings, arrays,
 * and nested structures.
 *
 * @param docblock Pointer to the documentation block structure to free
 *
 * @note If docblock is NULL, the function returns without doing anything
 * @note After calling this function, the documentation block structure will
 *       have all its pointers set to NULL and counters reset to zero
 * @note This function uses helper macros and functions to free different
 *       parts of the docblock structure
 */
void free_docblock(shellscribe_docblock_t *docblock) {
    if (docblock == NULL) return;
    FREE_STRING_FIELDS_3(docblock->file_name, docblock->brief, docblock->description);
    FREE_STRING_FIELDS_4(docblock->version, docblock->author, docblock->author_contact, docblock->project);
    FREE_STRING_FIELDS_2(docblock->license, docblock->copyright);
    FREE_FIELD(docblock->interpreter);
    FREE_STRING_FIELDS_3(docblock->function_name, docblock->function_description, docblock->function_brief);
    FREE_FIELD(docblock->alias);
    FREE_FIELD(docblock->return_desc);
    if (docblock->section) {
        FREE_STRING_FIELDS_2(docblock->section->name, docblock->section->description);
        shell_free((void**)&docblock->section);
    }
    FREE_ARRAY(docblock->arguments, docblock->arg_count, free_argument);
    FREE_ARRAY(docblock->params, docblock->param_count, free_param);
    FREE_ARRAY(docblock->returns, docblock->return_count, free_return);
    FREE_ARRAY(docblock->options, docblock->option_count, free_option);
    FREE_ARRAY(docblock->env_vars, docblock->env_var_count, free_env);
    FREE_ARRAY(docblock->see_also, docblock->see_also_count, free_seealso);
    FREE_ARRAY(docblock->alerts, docblock->alert_count, free_alert);
    FREE_ARRAY(docblock->set_vars, docblock->set_var_count, free_setvar);
    FREE_ARRAY(docblock->shellcheck_directives, docblock->shellcheck_count, free_shellcheck);
    struct {
        void ***field;
        int *count;
    } fields[] = {
        { (void ***)&docblock->warnings, &docblock->warning_count },
        { (void ***)&docblock->dependencies, &docblock->dependency_count },
        { (void ***)&docblock->internal_calls, &docblock->internal_call_count },
        { (void ***)&docblock->requires, &docblock->requires_count },
        { (void ***)&docblock->used_by, &docblock->used_by_count },
        { (void ***)&docblock->calls, &docblock->calls_count },
        { (void ***)&docblock->provides, &docblock->provides_count }
    };
    for (size_t i = 0; i < sizeof(fields) / sizeof(fields[0]); ++i) {
        for (int j = 0; j < *fields[i].count; ++j) {
            FREE_FIELD((*fields[i].field)[j]);
        }
        shell_free((void **)fields[i].field);
        *fields[i].field = NULL;
        *fields[i].count = 0;
    }
    FREE_STRING_FIELDS_3(docblock->stdin_doc, docblock->stdout_doc, docblock->stderr_doc);
    FREE_FIELD(docblock->example);
    FREE_STRING_FIELDS_3(docblock->deprecation.version, docblock->deprecation.replacement, docblock->deprecation.eol);
    docblock->deprecation.is_deprecated = false;
}

/**
 * @brief Free an array of documentation blocks
 *
 * This function frees all resources used by an array of documentation blocks.
 * It calls free_docblock() for each block in the array.
 *
 * @param docblocks Pointer to the array of documentation blocks to free
 * @param count Number of blocks in the array
 *
 * @note If docblocks is NULL or count is less than or equal to zero,
 *       the function returns without doing anything
 * @note This function only frees the contents of the docblocks, not the
 *       docblocks array itself
 * @note After calling this function, all docblocks in the array will have
 *       their resources freed, but the array itself will still need to be freed
 *       separately if it was dynamically allocated
 * 
 * @see free_docblock
 */
void free_docblocks(shellscribe_docblock_t *docblocks, int count) {
    if (docblocks == NULL || count <= 0) {
        return;
    }
    
    for (int i = 0; i < count; i++) {
        free_docblock(&docblocks[i]);
    }
} 
