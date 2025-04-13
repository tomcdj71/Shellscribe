/**
 * @file config.h
 * @brief Managing shellscribe configuration
 */

#ifndef SHELLSCRIBE_UTILS_CONFIG_H
#define SHELLSCRIBE_UTILS_CONFIG_H

#include <stdbool.h>

/**
 * @brief Structure representing a markdown style
 */
typedef struct {
 char *name;
 char *h1_from;
 char *h1_to;
 char *h2_from;
 char *h2_to;
 char *h3_from;
 char *h3_to;
 char *h4_from;
 char *h4_to;
 char *strong_from;
 char *strong_to;
 char *code_from;
 char *code_to;
 char *code_end;
 char *argN_from;
 char *argN_to;
 char *arg_at_from;
 char *arg_at_to;
 char *set_from;
 char *set_to;
 char *li_from;
 char *li_to;
 char *dt_from;
 char *dt_to;
 char *dd_from;
 char *dd_to;
 char *i_from;
 char *i_to;
 char *anchor_from;
 char *anchor_to;
 char *exitcode_from;
 char *exitcode_to;
} shellscribe_style_t;

/**
 * @brief log_level level enumeration
 */
typedef enum {
    log_level_MINIMAL,   // Only essential information
    log_level_NORMAL,    // Standard level of detail
    log_level_DETAILED,  // More detailed output
    log_level_DEBUG      // Full debug information
} log_level_level_t;

/**
 * @brief Memory tracking level enumeration
 */
typedef enum {
    MEMORY_TRACKING_NONE,     // No memory tracking
    MEMORY_TRACKING_BASIC,    // Basic allocation tracking
    MEMORY_TRACKING_FULL      // Detailed tracking with backtraces
} memory_tracking_level_t;

/**
 * @brief Output format enumeration
 */
typedef enum {
    FORMAT_MARKDOWN,      // Markdown format
    FORMAT_HTML,          // HTML format
    FORMAT_ASCIIDOC,      // AsciiDoc format
    FORMAT_TEXT           // Plain text format
} format_t;

/**
 * @brief Structure representing the shellscribe configuration
 */
typedef struct {
    // Debugging and memory management
    bool debug;
    bool verbose;
    bool no_output;
    bool memory_tracking;
    bool memory_stats;
    
    // Output configuration
    char *output_file;
    char *doc_path;
    char *doc_filename;
    char *format;
    bool generate_index;
    
    // Source file
    char *filename;
    
    // Metadata
    char *footer_text;
    char *version_placement;
    bool linkify_usernames;
    char *copyright_placement;
    char *license_placement;
    
    // log_level
    char *log_level_level;
    
    // Examples
    char *example_display;
    char *highlight_language;
    
    // Visual styling
    bool highlight_code;
    bool show_toc;
    bool show_alerts;
    bool show_shellcheck;
    char *arguments_display;
    char *shellcheck_display;    // Format d'affichage des directives shellcheck (table, sequential)
    
    // Behavior
    bool traverse_symlinks;
    
    // Style configuration
    shellscribe_style_t style;
} shellscribe_config_t;

/**
* @brief Loads the configuration from a specified file
*
* @param config Pointer to the configuration structure to populate
* @param config_file Path to the configuration file
* @return bool True if loading was successful, false otherwise
*/
bool load_config(shellscribe_config_t *config, const char *config_file);

/**
* @brief Frees the resources allocated for the configuration
*
* @param config Pointer to the configuration to free
*/
void free_config(shellscribe_config_t *config);

/**
* @brief Loads the settings from the .scribeconf file if it exists
*
* @param config Pointer to the configuration to update
* @return bool True if the .scribeconf was found and loaded, false otherwise
*/
bool load_scribeconf(shellscribe_config_t *config);

#endif /* SHELLSCRIBE_UTILS_CONFIG_H */
