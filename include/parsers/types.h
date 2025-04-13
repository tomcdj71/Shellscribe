/**
 * @file types.h
 * @brief Common data structures for shellscribe parsers
 */

#ifndef SHELLSCRIBE_PARSERS_COMMON_TYPES_H
#define SHELLSCRIBE_PARSERS_COMMON_TYPES_H

#include <stdio.h>
#include <stdbool.h>

/**
 * @brief Structure representing an argument 
 */
typedef struct {
    char *name;            // Name of the argument
    char *type;            // Type of the argument
    char *description;     // Description of the argument
} shellscribe_argument_t;

/**
 * @brief Structure representing an exit code
 */
typedef struct {
    char *code;            // Exit code value
    char *description;     // Description of the exit code
} shellscribe_exitcode_t;

/**
 * @brief Structure representing a shell option
 */
typedef struct {
    char *short_opt;       // Short option (e.g., -h)
    char *long_opt;        // Long option (e.g., --help)
    char *arg_spec;        // Argument specifier (e.g., FILE)
    char *description;     // Description of the option
} shellscribe_option_t;

/**
 * @brief Structure representing an environment variable
 */
typedef struct {
    char *name;            // Name of the environment variable
    char *default_value;   // Default value
    char *description;     // Description of the environment variable
} shellscribe_env_var_t;

/**
 * @brief Structure representing a parameter
 */
typedef struct {
    char *name;            // Name of the parameter
    char *description;     // Description of the parameter
} shellscribe_param_t;

/**
 * @brief Structure representing a return value
 */
typedef struct {
    char *value;           // Return value
    char *description;     // Description of the return value
} shellscribe_return_t;

/**
 * @brief Structure representing a "see also" reference
 */
typedef struct {
    char *name;            // Name of the referenced item
    char *url;             // URL for external references
    bool is_internal;      // Is this an internal reference?
} shellscribe_see_also_t;

/**
 * @brief Structure representing a global variable
 */
typedef struct {
    char *name;            // Name of the global variable
    char *type;            // Type of the variable (string, number, etc.)
    char *default_value;   // Default/initial value
    char *description;     // Description of the variable
    bool is_readonly;      // Whether the variable is read-only
} shellscribe_global_var_t;

/**
 * @brief Structure representing an alert
 */
typedef struct {
    char *type;        // Alert type: NOTE, TIP, IMPORTANT, WARNING, CAUTION
    char *content;     // Alert content
} shellscribe_alert_t;

/**
 * @brief Structure representing deprecation information
 */
typedef struct {
    bool is_deprecated;     // Whether the function is deprecated
    char *version;          // Version since when it's deprecated
    char *replacement;      // Replacement function
    char *eol;              // End-of-life version/date
} shellscribe_deprecation_t;

/**
 * @brief Structure representing a section in documentation
 */
typedef struct {
    char *name;             // Section name
    char *description;      // Section description
} shellscribe_section_t;

/**
 * @brief Structure to store shellcheck directive details
 */
typedef struct {
    char *code;       /**< The shellcheck error code (e.g., SC2034) */
    char *directive;  /**< The full shellcheck directive */
    char *reason;     /**< The reason provided for the directive (if any) */
} shellscribe_shellcheck_t;

/**
 * @brief Structure representing one documentation block
 */
typedef struct {
    // Script metadata
    char *file_name;
    char *brief;
    char *description;
    
    // Additional file metadata
    char *version;
    char *author;
    char *author_contact;
    char *project;
    char *license;
    char *copyright;
    char *interpreter;

    // Documented function
    char *function_name;
    char *function_description;
    char *function_brief;
    char *alias;            // Alternative name for the function
    char *return_desc;

    // Section information
    shellscribe_section_t *section;

    // Arguments
    shellscribe_argument_t *arguments;
    int arg_count;
    bool no_args;           // Explicitly marked as taking no arguments

    // Parameters (alternative to arguments)
    shellscribe_param_t *params;
    int param_count;

    // Return values
    shellscribe_return_t *returns;
    int return_count;

    // Inputs/outputs
    char *stdin_doc;
    char *stdout_doc;
    char *stderr_doc;

    // Return codes
    shellscribe_exitcode_t *exitcodes;
    int exitcode_count;

    // Options
    shellscribe_option_t *options;
    int option_count;

    // Environment variables
    shellscribe_env_var_t *env_vars;
    int env_var_count;

    // Examples
    char *example;

    // References (see also)
    shellscribe_see_also_t *see_also;
    int see_also_count;

    // Internal flags
    bool is_internal;
    bool is_skipped;         // Whether this file should be skipped in documentation generation

    // Deprecation info
    shellscribe_deprecation_t deprecation;

    // Alerts
    int alert_count;
    shellscribe_alert_t *alerts;
    
    // Warnings
    char **warnings;
    int warning_count;
    
    // Dependencies
    char **dependencies;
    int dependency_count;
    
    // Internal calls
    char **internal_calls;
    int internal_call_count;
    
    // Required dependencies (@requires)
    char **requires;
    int requires_count;
    
    // Functions using this function (@used-by)
    char **used_by;
    int used_by_count;
    
    // External function calls (@calls)
    char **calls;
    int calls_count;
    
    // Services/features provided (@provides)
    char **provides;
    int provides_count;
    
    // Global variables set by the function
    shellscribe_global_var_t *set_vars;
    int set_var_count;
    
    // Shellcheck directives
    shellscribe_shellcheck_t *shellcheck_directives;
    int shellcheck_count;
} shellscribe_docblock_t;

/**
 * @brief Memory management functions for documentation blocks
 */

/**
 * @brief Initialize a documentation block with default values
 * 
 * @param docblock Pointer to the documentation block to initialize
 */
void init_docblock(shellscribe_docblock_t *docblock);

/**
 * @brief Free memory associated with a documentation block
 * 
 * @param docblock Pointer to the documentation block to free
 */
void free_docblock(shellscribe_docblock_t *docblock);

/**
 * @brief Free an array of documentation blocks
 * 
 * @param docblocks Array of documentation blocks
 * @param count Number of blocks in the array
 */
void free_docblocks(shellscribe_docblock_t *docblocks, int count);

#endif /* SHELLSCRIBE_PARSERS_COMMON_TYPES_H */ 
