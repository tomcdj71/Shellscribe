/**
 * @file config.c
 * @brief Configuration management for Shellscribe
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>

#include "utils/config.h"
#include "utils/memory.h"
#include "utils/string.h"
#include "utils/debug.h"

// Define SHELLSCRIBE_VERSION if not already defined
#ifndef SHELLSCRIBE_VERSION
#define SHELLSCRIBE_VERSION "1.0.0"
#endif

static const char *CONFIG_TEMPLATES_DIR = "templates";
static const char *APP_NAME = "shellscribe";

/**
 * @brief Initializes the GitHub style in the provided style structure
 * 
 * @param style Pointer to the style structure to initialize with GitHub theme
 */
static void init_github_style(shellscribe_style_t *style) {
    if (style == NULL) {
        return;
    }
    style->name = string_duplicate("github");
    style->h1_from = string_duplicate("# ");
    style->h1_to = string_duplicate("\n\n");
    style->h2_from = string_duplicate("## ");
    style->h2_to = string_duplicate("\n\n");
    style->h3_from = string_duplicate("### ");
    style->h3_to = string_duplicate("\n\n");
    style->h4_from = string_duplicate("#### ");
    style->h4_to = string_duplicate("\n\n");
    style->strong_from = string_duplicate("**");
    style->strong_to = string_duplicate("**");
    style->i_from = string_duplicate("*");
    style->i_to = string_duplicate("*");
    style->code_from = string_duplicate("```bash\n");
    style->code_to = string_duplicate("\n");
    style->code_end = string_duplicate("```\n");
    style->argN_from = string_duplicate("`$");
    style->argN_to = string_duplicate("`");
    style->arg_at_from = string_duplicate("`$@`");
    style->arg_at_to = string_duplicate("");
    style->set_from = string_duplicate("`");
    style->set_to = string_duplicate("`");
    style->li_from = string_duplicate("- ");
    style->li_to = string_duplicate("\n");
    style->dt_from = string_duplicate("**");
    style->dt_to = string_duplicate("**: ");
    style->dd_from = string_duplicate("");
    style->dd_to = string_duplicate("\n");
    style->anchor_from = string_duplicate("[");
    style->anchor_to = string_duplicate("]");
    style->exitcode_from = string_duplicate("`");
    style->exitcode_to = string_duplicate("`");
}

/**
 * @brief Initializes a dark style in the provided style structure
 * 
 * @param style Pointer to the style structure to initialize with dark theme
 */
static void init_dark_style(shellscribe_style_t *style) {
    if (style == NULL) {
        return;
    }
    init_github_style(style);
    style->name = string_duplicate("dark");
}

/**
 * @brief Initializes a light style in the provided style structure
 * 
 * @param style Pointer to the style structure to initialize with light theme
 */
static void init_light_style(shellscribe_style_t *style) {
    if (style == NULL) {
        return;
    }
    init_github_style(style);
    style->name = string_duplicate("light");
}

/**
 * @brief Gets the default config file path for the application
 * 
 * @return char* The path to the default config file
 */
char *get_default_config_file() {
    char *config_home = getenv("XDG_CONFIG_HOME");
    char *home = NULL;
    char *config_path = NULL;
    if (config_home != NULL && strlen(config_home) > 0) {
        config_path = string_concat(config_home, "/");
        char *temp = config_path;
        config_path = string_concat(config_path, APP_NAME);
        shell_free((void **)&temp);
    } else {
        home = getenv("HOME");
        if (home == NULL) {
            struct passwd *pw = getpwuid(getuid());
            if (pw != NULL) {
                home = pw->pw_dir;
            } else {
                return NULL;
            }
        }
        config_path = string_concat(home, "/.config/");
        char *temp = config_path;
        config_path = string_concat(config_path, APP_NAME);
        shell_free((void **)&temp);
    }
    struct stat st = {0};
    if (stat(config_path, &st) == -1) {
        mkdir(config_path, 0700);
    }
    char *config_file = string_concat(config_path, "/config.json");
    shell_free((void **)&config_path);
    
    return config_file;
}

/**
 * @brief Gets the template directory path for the application
 * 
 * @return char* The path to the templates directory or NULL if not found
 */
char *get_templates_dir() {
    char *templates_dir = string_concat("./", CONFIG_TEMPLATES_DIR);
    struct stat st = {0};
    if (stat(templates_dir, &st) == 0 && S_ISDIR(st.st_mode)) {
        return templates_dir;
    }
    shell_free((void **)&templates_dir);
    char *config_file = get_default_config_file();
    if (config_file == NULL) {
        return NULL;
    }
    char *config_dir = NULL;
    char *last_slash = strrchr(config_file, '/');
    if (last_slash != NULL) {
        size_t dir_len = last_slash - config_file;
        config_dir = shell_malloc(dir_len + 1);
        strncpy(config_dir, config_file, dir_len);
        config_dir[dir_len] = '\0';
    }
    shell_free((void **)&config_file);
    if (config_dir != NULL) {
        templates_dir = string_concat(config_dir, "/");
        shell_free((void **)&config_dir);
        char *temp = templates_dir;
        templates_dir = string_concat(templates_dir, CONFIG_TEMPLATES_DIR);
        shell_free((void **)&temp);
        
        if (stat(templates_dir, &st) == 0 && S_ISDIR(st.st_mode)) {
            return templates_dir;
        }
        shell_free((void **)&templates_dir);
    }
    templates_dir = string_duplicate("/usr/share/shellscribe/templates");
    if (stat(templates_dir, &st) == 0 && S_ISDIR(st.st_mode)) {
        return templates_dir;
    }
    shell_free((void **)&templates_dir);
    
    return NULL;
}

/**
 * @brief Free allocated memory for style
 * @param style Pointer to style structure
 */
static void free_style(shellscribe_style_t *style) {
    if (style == NULL) {
        return;
    }
    char **fields[] = {
        &style->name,
        &style->h1_from, &style->h1_to, &style->h2_from, &style->h2_to,
        &style->h3_from, &style->h3_to, &style->h4_from, &style->h4_to,
        &style->strong_from, &style->strong_to, &style->i_from, &style->i_to,
        &style->code_from, &style->code_to, &style->code_end,
        &style->argN_from, &style->argN_to, &style->arg_at_from, &style->arg_at_to,
        &style->set_from, &style->set_to,
        &style->li_from, &style->li_to, &style->dt_from, &style->dt_to,
        &style->dd_from, &style->dd_to,
        &style->anchor_from, &style->anchor_to,
        &style->exitcode_from, &style->exitcode_to
    };
    for (size_t i = 0; i < sizeof(fields) / sizeof(fields[0]); i++) {
        if (*fields[i] != NULL) {
            free(*fields[i]);
            *fields[i] = NULL;
        }
    }
}

/**
 * @brief Load configuration from a file
 * @param config Pointer to configuration structure
 * @param config_file Path to configuration file (or NULL for default)
 * @return true if successful, false otherwise
 */
bool load_config_from_file(shellscribe_config_t *config, const char *config_file) {
    if (config_file == NULL) {
        return false;
    }
    FILE *file = fopen(config_file, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open configuration file: %s\n", config_file);
        return false;
    }
    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }
        char *sep = strchr(line, '=');
        if (sep == NULL) {
            continue;
        }
        *sep = '\0';
        char *key = string_trim(line);
        char *value = string_trim(sep + 1);
        if (key == NULL || value == NULL) {
            free(key);
            free(value);
            continue;
        }
        if (strcmp(key, "footer_text") == 0) {
            free(config->footer_text);
            config->footer_text = value;
            value = NULL;
        }
        free(key);
        free(value);
    }

    fclose(file);
    return true;
}

/**
 * @brief Load configuration
 * @param config Pointer to configuration structure
 * @param config_file Path to configuration file (or NULL for default)
 * @return true if successful, false otherwise
 */
bool load_config(shellscribe_config_t *config, const char *config_file) {
    if (config == NULL) {
        return false;
    }
    *config = (shellscribe_config_t){
        .debug = false,
        .verbose = false,
        .no_output = false,
        .memory_tracking = false,
        .memory_stats = false,
        .output_file = NULL,
        .doc_path = shell_strdup("./docs"),
        .doc_filename = shell_strdup("shell_doc"),
        .format = shell_strdup("markdown"),
        .generate_index = false,
        .filename = NULL,
        .footer_text = NULL,
        .version_placement = shell_strdup("about"),
        .linkify_usernames = false,
        .copyright_placement = shell_strdup("pre-footer"),
        .license_placement = shell_strdup("pre-footer"),
        .log_level_level = shell_strdup("normal"),
        .example_display = shell_strdup("sequential"),
        .highlight_language = shell_strdup("bash"),
        .highlight_code = true,
        .show_toc = true,
        .show_alerts = false,
        .show_shellcheck = false,
        .arguments_display = shell_strdup("sequential"),
        .shellcheck_display = shell_strdup("sequential"),
        .traverse_symlinks = true
    };
    char footer_buffer[256];
    snprintf(footer_buffer, sizeof(footer_buffer), 
                "This documentation was auto generated with [Shellscribe](https://github.com/tomcdj71/shellscribe) (v%s)", 
                SHELLSCRIBE_VERSION);
    config->footer_text = shell_strdup(footer_buffer);
    init_github_style(&config->style);
    if (config_file != NULL) {
        if (load_config_from_file(config, config_file)) {
            fprintf(stderr, "Loaded configuration from: %s\n", config_file);
        } else {
            fprintf(stderr, "Warning: Could not load config file, using defaults\n");
        }
    } else {
        load_scribeconf(config);
    }

    return true;
}

/**
 * @brief Free allocated memory for configuration
 * @param config Pointer to configuration structure
 */
void free_config(shellscribe_config_t *config) {
    if (config == NULL) {
        return;
    }
    free_style(&config->style);
    char *fields[] = {
        config->footer_text, config->output_file, config->doc_path, config->doc_filename,
        config->format, config->log_level_level, config->example_display,
        config->highlight_language, config->copyright_placement,
        config->license_placement, config->version_placement, config->arguments_display,
        config->shellcheck_display
    };

    for (size_t i = 0; i < sizeof(fields) / sizeof(fields[0]); i++) {
        free(fields[i]);
    }
}

static void set_memory_tracking(shellscribe_config_t *cfg, const char *val) {
    cfg->memory_tracking = (strcmp(val, "true") == 0);
}

static void set_memory_stats(shellscribe_config_t *cfg, const char *val) {
    cfg->memory_stats = (strcmp(val, "true") == 0);
}

static void set_doc_path(shellscribe_config_t *cfg, const char *val) {
    free(cfg->doc_path);
    cfg->doc_path = string_duplicate(val);
}

static void set_doc_filename(shellscribe_config_t *cfg, const char *val) {
    free(cfg->doc_filename);
    cfg->doc_filename = string_duplicate(val);
}

static void set_format(shellscribe_config_t *cfg, const char *val) {
    free(cfg->format);
    cfg->format = string_duplicate(val);
}

static void set_generate_index(shellscribe_config_t *cfg, const char *val) {
    cfg->generate_index = (strcmp(val, "true") == 0);
}

static void set_footer_text(shellscribe_config_t *cfg, const char *val) {
    free(cfg->footer_text);
    cfg->footer_text = string_duplicate(val);
}

static void set_version_placement(shellscribe_config_t *cfg, const char *val) {
    free(cfg->version_placement);
    cfg->version_placement = string_duplicate(val);
}

static void set_linkify_usernames(shellscribe_config_t *cfg, const char *val) {
    cfg->linkify_usernames = (strcmp(val, "true") == 0);
}

static void set_copyright_placement(shellscribe_config_t *cfg, const char *val) {
    free(cfg->copyright_placement);
    cfg->copyright_placement = string_duplicate(val);
}

static void set_license_placement(shellscribe_config_t *cfg, const char *val) {
    free(cfg->license_placement);
    cfg->license_placement = string_duplicate(val);
}

static void set_log_level(shellscribe_config_t *cfg, const char *val) {
    free(cfg->log_level_level);
    cfg->log_level_level = string_duplicate(val);
    cfg->verbose = (strcmp(val, "verbose") == 0);
}

static void set_example_display(shellscribe_config_t *cfg, const char *val) {
    free(cfg->example_display);
    cfg->example_display = string_duplicate(val);
}

static void set_highlight_language(shellscribe_config_t *cfg, const char *val) {
    free(cfg->highlight_language);
    cfg->highlight_language = string_duplicate(val);
}

static void set_highlight_code(shellscribe_config_t *cfg, const char *val) {
    cfg->highlight_code = (strcmp(val, "true") == 0);
}

static void set_show_toc(shellscribe_config_t *cfg, const char *val) {
    cfg->show_toc = (strcmp(val, "true") == 0);
}

static void set_show_alerts(shellscribe_config_t *cfg, const char *val) {
    cfg->show_alerts = (strcmp(val, "true") == 0);
}

static void set_show_shellcheck(shellscribe_config_t *cfg, const char *val) {
    cfg->show_shellcheck = (strcmp(val, "true") == 0);
}

static void set_arguments_display(shellscribe_config_t *cfg, const char *val) {
    free(cfg->arguments_display);
    cfg->arguments_display = string_duplicate(val);
}

static void set_traverse_symlinks(shellscribe_config_t *cfg, const char *val) {
    cfg->traverse_symlinks = (strcmp(val, "true") == 0);
}

static void set_shellcheck_display(shellscribe_config_t *cfg, const char *val) {
    free(cfg->shellcheck_display);
    cfg->shellcheck_display = string_duplicate(val);
}

/**
 * @brief Load scribe configuration from file
 * 
 * This function loads the scribe configuration from a file named ".scribeconf".
 */
bool load_scribeconf(shellscribe_config_t *config) {
    FILE *file = fopen("./.scribeconf", "r");
    if (file == NULL) {
        return false;
    }

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }
        char *sep = strchr(line, '=');
        if (sep == NULL) {
            continue;
        }
        *sep = '\0';
        char *key = string_trim(line);
        char *value = string_trim(sep + 1);
        if (key == NULL || value == NULL) {
            free(key);
            free(value);
            continue;
        }
        char *comment = strchr(value, '#');
        if (comment != NULL) {
            *comment = '\0';
            value = string_trim(value);
        }
        struct {
            const char *key;
            void (*action)(shellscribe_config_t *, const char *);
        } config_map[] = {
            {"memory_tracking", set_memory_tracking},
            {"memory_stats", set_memory_stats},
            {"doc_path", set_doc_path},
            {"doc_filename", set_doc_filename},
            {"format", set_format},
            {"generate_index", set_generate_index},
            {"footer_text", set_footer_text},
            {"version_placement", set_version_placement},
            {"linkify_usernames", set_linkify_usernames},
            {"copyright_placement", set_copyright_placement},
            {"license_placement", set_license_placement},
            {"log_level", set_log_level},
            {"example_display", set_example_display},
            {"highlight_language", set_highlight_language},
            {"highlight_code", set_highlight_code},
            {"show_toc", set_show_toc},
            {"show_alerts", set_show_alerts},
            {"show_shellcheck", set_show_shellcheck},
            {"arguments_display", set_arguments_display},
            {"shellcheck_display", set_shellcheck_display},
            {"traverse_symlinks", set_traverse_symlinks},
        };
        bool handled = false;
        for (size_t i = 0; i < sizeof(config_map) / sizeof(config_map[0]); i++) {
            if (strcmp(key, config_map[i].key) == 0) {
                config_map[i].action(config, value);
                handled = true;
                break;
            }
        }
        if (!handled) {
            fprintf(stderr, "Warning: Unknown configuration key: %s\n", key);
        }
        free(key);
        free(value);
    }

    fclose(file);
    return true;
}

/**
 * @brief Set default values in shellscribe config structure
 */
static void set_defaults(shellscribe_config_t *config) {
    config->format                     = string_duplicate("markdown");
    config->generate_index             = false;
    config->doc_path                   = string_duplicate("./docs");
    config->doc_filename               = string_duplicate("README.md");
    config->highlight_language         = string_duplicate("bash");
    config->footer_text                = string_duplicate("Documentation generated by ShellScribe");
    config->version_placement          = string_duplicate("footer");
    config->highlight_code             = true;
    config->show_toc                   = true;
    config->show_alerts                = true;
    config->show_shellcheck            = true;
    config->linkify_usernames          = true;
    config->copyright_placement        = string_duplicate("none");
    config->license_placement          = string_duplicate("none");
    config->memory_tracking            = true;
    config->memory_stats               = false;
    config->log_level_level            = string_duplicate("normal");
    config->example_display            = string_duplicate("sequential");
    config->arguments_display          = string_duplicate("table");
    config->shellcheck_display         = string_duplicate("table");
    config->traverse_symlinks          = false;
}
