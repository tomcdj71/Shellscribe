/**
 * @file docblock.c
 * @brief Implementation of docblock renderer for individual function documentation
 *
 * This file contains functions for rendering documentation blocks into
 * formatted Markdown documentation. Each function handles a specific part
 * of the documentation rendering process, such as rendering function descriptions,
 * examples, arguments, dependencies, and other documentation elements.
 */

#include "renderers/docblock.h"
#include "renderers/renderer_engine.h"
#include "renderers/style.h"
#include "utils/string.h"
#include "utils/debug.h"
#include "core/model.h"
#include "utils/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define EXAMPLE_BUFFER_SIZE 1024

/**
 * @brief Write an example code snippet to output with proper formatting
 * 
 * Formats and writes a code example to the output stream. The example can be
 * rendered either as a simple code block or as a collapsible details/summary
 * section with a numbered title.
 * 
 * @param example The example code text to write
 * @param output File stream where the example will be written
 * @param with_details Whether to wrap the example in a details/summary element
 * @param index Index of the example (used for numbering if with_details is true)
 * @param config Configuration settings for controlling example formatting
 * 
 * @note The first example (index=0) is rendered with the "open" attribute if with_details is true
 * @note The language used for syntax highlighting is determined by config->highlight_language
 */
static void write_example(const char *example, FILE *output, bool with_details, int index, const shellscribe_config_t *config) {
    if (example == NULL || output == NULL) {
        return;
    }
    const char *lang = "";
    if (config && config->highlight_code) {
        lang = config->highlight_language ? config->highlight_language : "bash";
    }
    if (with_details) {
        fprintf(output, "<details%s>\n<summary>Example %d</summary>\n\n```%s\n  %s\n```\n\n</details>\n", 
                index == 0 ? " open" : "", index + 1, lang, example);
    } else {
        fprintf(output, "```%s\n  %s\n```\n\n", lang, example);
    }
}

/**
 * @brief Render examples in a memory-efficient way
 * 
 * Processes and renders example code snippets from a documentation block.
 * This function handles multiple examples (separated by blank lines) and
 * renders them according to the configured display style (sequential or tabs).
 * 
 * @param docblock Documentation block containing the examples to render
 * @param output File stream where the examples will be written
 * @param config Configuration settings for controlling example display
 * 
 * @note Uses a fixed-size buffer to process examples to avoid excessive memory usage
 * @note Multiple examples are detected by the presence of "\n\n" in the example field
 * @note If configured for "tabs" display, examples are wrapped in details/summary elements
 */
static void render_examples_efficient(const shellscribe_docblock_t *docblock, FILE *output, const shellscribe_config_t *config) {
    if (docblock == NULL || docblock->example == NULL || output == NULL) {
        return;
    }
    bool has_multiple = (strstr(docblock->example, "\n\n") != NULL);
    if (has_multiple) {
        fprintf(output, "#### Examples\n\n");
        bool use_tabs = config && (strcmp(config->example_display, "tabs") == 0);
        if (use_tabs) {
            fprintf(output, "<div class=\"example-tabs\">\n");
        }
        const char *example_start = docblock->example;
        const char *example_end;
        char buffer[EXAMPLE_BUFFER_SIZE];
        int example_index = 0;
        while (example_start && *example_start) {
            example_end = strstr(example_start, "\n\n");
            if (example_end) {
                size_t len = example_end - example_start;
                if (len >= EXAMPLE_BUFFER_SIZE) {
                    len = EXAMPLE_BUFFER_SIZE - 1;
                }
                strncpy(buffer, example_start, len);
                buffer[len] = '\0';
                write_example(buffer, output, use_tabs, example_index++, config);
                example_start = example_end + 2;
            } else {
                write_example(example_start, output, use_tabs, example_index, config);
                break;
            }
        }
        if (use_tabs) {
            fprintf(output, "</div>\n\n");
        }
    } else {
        fprintf(output, "#### Example\n\n");
        write_example(docblock->example, output, false, 0, config);
    }
}

/**
 * @brief Renders the arguments or parameters section of a docblock
 * 
 * This function renders either the arguments section (from arg_count/arguments)
 * or the parameters section (from param_count/params) of a documentation block.
 * The output format can be either a list or a table, depending on the configuration.
 * 
 * @param docblock Documentation block containing the arguments/parameters to render
 * @param output File stream where the arguments/parameters will be written
 * @param config Configuration settings for controlling argument/parameter display
 * 
 * @note Arguments have name, type, and description fields
 * @note Parameters have only name and description fields
 * @note The display format (list or table) is controlled by config->arguments_display
 */
void render_arguments(const shellscribe_docblock_t *docblock, FILE *output, const shellscribe_config_t *config) {
    if (docblock == NULL || output == NULL || config == NULL) {
        return;
    }
    if (docblock->arg_count > 0) {
        fprintf(output, "#### Arguments\n\n");
        if (config->arguments_display && strcmp(config->arguments_display, "table") == 0) {
            fprintf(output, "| Argument | Type | Description |\n");
            fprintf(output, "|----------|------|-------------|\n");
            for (int i = 0; i < docblock->arg_count; i++) {
                fprintf(output, "| %s | %s | %s |\n", 
                        docblock->arguments[i].name ? docblock->arguments[i].name : "", 
                        docblock->arguments[i].type ? docblock->arguments[i].type : "",
                        docblock->arguments[i].description ? docblock->arguments[i].description : "");
            }
        } else {
            for (int i = 0; i < docblock->arg_count; i++) {
                fprintf(output, "* %s (%s)\n  %s\n", 
                        docblock->arguments[i].name ? docblock->arguments[i].name : "", 
                        docblock->arguments[i].type ? docblock->arguments[i].type : "",
                        docblock->arguments[i].description ? docblock->arguments[i].description : "");
            }
        }
        fprintf(output, "\n");
    } else if (docblock->param_count > 0) {
        fprintf(output, "#### Parameters\n\n");
        
        if (config->arguments_display && strcmp(config->arguments_display, "table") == 0) {
            fprintf(output, "| Parameter | Description |\n");
            fprintf(output, "|-----------|-------------|\n");
            for (int i = 0; i < docblock->param_count; i++) {
                fprintf(output, "| `%s` | %s |\n", 
                        docblock->params[i].name ? docblock->params[i].name : "", 
                        docblock->params[i].description ? docblock->params[i].description : "");
            }
        } else {
            for (int i = 0; i < docblock->param_count; i++) {
                fprintf(output, "* `%s`: %s\n", 
                        docblock->params[i].name ? docblock->params[i].name : "", 
                        docblock->params[i].description ? docblock->params[i].description : "");
            }
        }
        fprintf(output, "\n");
    }
}

/**
 * @brief Render a complete documentation block
 * 
 * This function renders an entire documentation block, including the function name,
 * description, alerts, examples, arguments, dependencies, return values, stdout
 * documentation, and shellcheck exceptions.
 * 
 * @param docblock Documentation block to render
 * @param output File stream where the documentation will be written
 * @param config Configuration settings for controlling documentation rendering
 * 
 * @note Skips rendering if docblock or output is NULL, or if the function_name is NULL
 * @note Delegates rendering of specific sections to specialized functions
 */
void render_docblock(const shellscribe_docblock_t *docblock,
                    FILE *output, const shellscribe_config_t *config) {
    if (docblock == NULL || output == NULL || config == NULL) {
        return;
    }
    if (docblock->function_name == NULL) {
        return;
    }
    char *anchor = create_anchor_link(docblock->function_name);
    fprintf(output, "\n### %s\n\n", docblock->function_name);
    if (docblock->function_brief != NULL) {
        fprintf(output, "%s\n", docblock->function_brief);
    } else if (docblock->brief != NULL) {
        fprintf(output, "%s\n", docblock->brief);
    }
    if (docblock->function_description != NULL) {
        fprintf(output, "%s\n\n", docblock->function_description);
    } else if (docblock->description != NULL) {
        fprintf(output, "%s\n\n", docblock->description);
    }
    if (docblock->alert_count > 0 && config->show_alerts) {
        for (int i = 0; i < docblock->alert_count; i++) {
            const char *type = docblock->alerts[i].type ? docblock->alerts[i].type : "NOTE";
            const char *content = docblock->alerts[i].content ? docblock->alerts[i].content : "";
            char *type_upper = string_duplicate(type);
            if (type_upper) {
                for (size_t j = 0; j < strlen(type_upper); j++) {
                    type_upper[j] = toupper(type_upper[j]);
                }
            }
            fprintf(output, "> **%s:**  \n", type_upper ? type_upper : "NOTE");
            free(type_upper);
            char *content_copy = string_duplicate(content);
            char *line_context = NULL;
            char *line = strtok_r(content_copy, "\n", &line_context);
            while (line != NULL) {
                fprintf(output, "> %s \n", line);
                line = strtok_r(NULL, "\n", &line_context);
            }
            fprintf(output, "\n");
            free(content_copy);
        }
    }
    if (docblock->example != NULL) {
        render_examples_efficient(docblock, output, config);
    }
    render_arguments(docblock, output, config);
    render_dependencies(docblock, output, config);
    if (docblock->return_count > 0 || docblock->return_desc != NULL) {
        fprintf(output, "#### Return Values\n\n");
        if (docblock->return_desc != NULL) {
            fprintf(output, "%s\n\n", docblock->return_desc);
        }
        for (int i = 0; i < docblock->return_count; i++) {
            fprintf(output, "* %s\n", 
                    docblock->returns[i].description ? docblock->returns[i].description : "");
        }
        fprintf(output, "\n");
    }
    if (docblock->stdout_doc != NULL) {
        fprintf(output, "#### Output on stdout\n");
        fprintf(output, "* %s\n\n", docblock->stdout_doc);
    }
    if (docblock->shellcheck_count > 0) {
        fprintf(output, "#### Shellcheck Exceptions\n\n");
        char **displayed_codes = (char **)calloc(docblock->shellcheck_count, sizeof(char *));
        int displayed_count = 0;
        bool has_any_reason = false;
        for (int i = 0; i < docblock->shellcheck_count; i++) {
            const char *code = docblock->shellcheck_directives[i].code ? docblock->shellcheck_directives[i].code : "";
            const char *reason = docblock->shellcheck_directives[i].reason ? docblock->shellcheck_directives[i].reason : "";
            bool duplicate = false;
            for (int j = 0; j < displayed_count; j++) {
                if (displayed_codes[j] && strcmp(displayed_codes[j], code) == 0) {
                    duplicate = true;
                    break;
                }
            }
            if (!duplicate && *code) {
                displayed_codes[displayed_count++] = strdup(code);
                if (reason && *reason) {
                    has_any_reason = true;
                }
            }
        }
        const char *display_mode = config->shellcheck_display ? config->shellcheck_display : "sequential";
        if (strcmp(display_mode, "table") == 0) {
            if (has_any_reason) {
                fprintf(output, "| Code | Reason |\n");
                fprintf(output, "|------|--------|\n");
            } else {
                fprintf(output, "| Code |\n");
                fprintf(output, "|------|\n");
            }
            for (int i = 0; i < docblock->shellcheck_count; i++) {
                const char *code = docblock->shellcheck_directives[i].code ? docblock->shellcheck_directives[i].code : "";
                const char *reason = docblock->shellcheck_directives[i].reason ? docblock->shellcheck_directives[i].reason : "";
                bool already_displayed = false;
                for (int j = 0; j < i; j++) {
                    const char *prev_code = docblock->shellcheck_directives[j].code ? docblock->shellcheck_directives[j].code : "";
                    if (strcmp(prev_code, code) == 0) {
                        already_displayed = true;
                        break;
                    }
                }                
                if (!already_displayed && *code) {
                    if (strncmp(code, "SC", 2) == 0) {
                        if (has_any_reason) {
                            fprintf(output, "| [%s](https://www.shellcheck.net/wiki/%s) | %s |\n", 
                                    code, code, reason);
                        } else {
                            fprintf(output, "| [%s](https://www.shellcheck.net/wiki/%s) |\n", 
                                    code, code);
                        }
                    } else {
                        if (has_any_reason) {
                            fprintf(output, "| %s | %s |\n", code, reason);
                        } else {
                            fprintf(output, "| %s |\n", code);
                        }
                    }
                }
            }
        } else if (strcmp(display_mode, "sequential") == 0) {
            for (int i = 0; i < docblock->shellcheck_count; i++) {
                const char *code = docblock->shellcheck_directives[i].code ? docblock->shellcheck_directives[i].code : "";
                const char *reason = docblock->shellcheck_directives[i].reason ? docblock->shellcheck_directives[i].reason : "";
                bool already_displayed = false;
                for (int j = 0; j < i; j++) {
                    const char *prev_code = docblock->shellcheck_directives[j].code ? docblock->shellcheck_directives[j].code : "";
                    if (strcmp(prev_code, code) == 0) {
                        already_displayed = true;
                        break;
                    }
                }
                if (!already_displayed && *code) {
                    if (strncmp(code, "SC", 2) == 0) {
                        fprintf(output, "[%s](https://www.shellcheck.net/wiki/%s)", code, code);
                    } else {
                        fprintf(output, "[%s]", code);
                    }
                    if (reason && *reason) {
                        fprintf(output, " (%s)", reason);
                    }
                    fprintf(output, "\n");
                }
            }
        } else {
            for (int i = 0; i < docblock->shellcheck_count; i++) {
                const char *code = docblock->shellcheck_directives[i].code ? docblock->shellcheck_directives[i].code : "";
                const char *reason = docblock->shellcheck_directives[i].reason ? docblock->shellcheck_directives[i].reason : "";
                bool already_displayed = false; 
                for (int j = 0; j < i; j++) {
                    const char *prev_code = docblock->shellcheck_directives[j].code ? docblock->shellcheck_directives[j].code : "";
                    if (strcmp(prev_code, code) == 0) {
                        already_displayed = true;
                        break;
                    }
                }                
                if (!already_displayed && *code) {
                    if (strncmp(code, "SC", 2) == 0) {
                        fprintf(output, "* [%s](https://www.shellcheck.net/wiki/%s)", code, code);
                    } else {
                        fprintf(output, "* %s", code);
                    }
                    if (reason && *reason) {
                        fprintf(output, " - %s", reason);
                    }                    
                    fprintf(output, "\n");
                }
            }
        }
        for (int i = 0; i < displayed_count; i++) {
            free(displayed_codes[i]);
        }
        free(displayed_codes);
        fprintf(output, "\n");
    }
}

/**
 * @brief Renders the dependencies section of a docblock
 *
 * This function renders the various types of dependencies associated with a function,
 * including required dependencies, functions that use this function, external calls,
 * provided services/features, and other dependencies. Each category is displayed
 * in its own subsection if there are entries for it.
 *
 * @param docblock Documentation block containing the dependency information
 * @param output File stream where the dependencies will be written
 * @param config Configuration settings for controlling dependency display
 * 
 * @note If there are no dependencies of any type, this function does nothing
 * @note The display format (list or table) is controlled by config->arguments_display
 * @note Dependency types include: required dependencies, used by, external calls,
 *       provides, and other dependencies (including internal calls)
 */
void render_dependencies(const shellscribe_docblock_t *docblock, FILE *output, const shellscribe_config_t *config) {
    if (docblock == NULL || output == NULL || config == NULL) {
        return;
    }
    if (docblock->requires_count == 0 && 
        docblock->used_by_count == 0 && 
        docblock->calls_count == 0 && 
        docblock->provides_count == 0 &&
        docblock->dependency_count == 0 && 
        docblock->internal_call_count == 0) {
        return;
    }
    
    fprintf(output, "#### Dependencies\n\n");
    if (docblock->requires_count > 0) {
        fprintf(output, "##### Required Dependencies\n\n");
        if (config->arguments_display && strcmp(config->arguments_display, "table") == 0) {
            fprintf(output, "| Name |\n");
            fprintf(output, "|------|\n");
            for (int i = 0; i < docblock->requires_count; i++) {
                fprintf(output, "| `%s` |\n", 
                        docblock->requires[i] ? docblock->requires[i] : "");
            }
        } else {
            for (int i = 0; i < docblock->requires_count; i++) {
                fprintf(output, "* `%s`\n", 
                        docblock->requires[i] ? docblock->requires[i] : "");
            }
        }
        fprintf(output, "\n");
    }
    if (docblock->used_by_count > 0) {
        fprintf(output, "##### Used By\n\n");
        if (config->arguments_display && strcmp(config->arguments_display, "table") == 0) {
            fprintf(output, "| Function |\n");
            fprintf(output, "|---------|\n");
            for (int i = 0; i < docblock->used_by_count; i++) {
                fprintf(output, "| `%s` |\n", 
                        docblock->used_by[i] ? docblock->used_by[i] : "");
            }
        } else {
            for (int i = 0; i < docblock->used_by_count; i++) {
                fprintf(output, "* `%s`\n", 
                        docblock->used_by[i] ? docblock->used_by[i] : "");
            }
        }
        fprintf(output, "\n");
    }
    if (docblock->calls_count > 0) {
        fprintf(output, "##### External Calls\n\n");
        if (config->arguments_display && strcmp(config->arguments_display, "table") == 0) {
            fprintf(output, "| Command/Function |\n");
            fprintf(output, "|----------------|\n");
            for (int i = 0; i < docblock->calls_count; i++) {
                fprintf(output, "| `%s` |\n", 
                        docblock->calls[i] ? docblock->calls[i] : "");
            }
        } else {
            for (int i = 0; i < docblock->calls_count; i++) {
                fprintf(output, "* `%s`\n", 
                        docblock->calls[i] ? docblock->calls[i] : "");
            }
        }
        fprintf(output, "\n");
    }
    if (docblock->provides_count > 0) {
        fprintf(output, "##### Provides\n\n");
        if (config->arguments_display && strcmp(config->arguments_display, "table") == 0) {
            fprintf(output, "| Service/Feature |\n");
            fprintf(output, "|----------------|\n");
            for (int i = 0; i < docblock->provides_count; i++) {
                fprintf(output, "| %s |\n", 
                        docblock->provides[i] ? docblock->provides[i] : "");
            }
        } else {
            for (int i = 0; i < docblock->provides_count; i++) {
                fprintf(output, "* %s\n", 
                        docblock->provides[i] ? docblock->provides[i] : "");
            }
        }
        fprintf(output, "\n");
    }
    if (docblock->dependency_count > 0 || docblock->internal_call_count > 0) {
        fprintf(output, "##### Other Dependencies\n\n");
        if (config->arguments_display && strcmp(config->arguments_display, "table") == 0) {
            fprintf(output, "| Name | Type |\n");
            fprintf(output, "|------|------|\n");
            for (int i = 0; i < docblock->dependency_count; i++) {
                fprintf(output, "| `%s` | Dependency |\n", 
                        docblock->dependencies[i] ? docblock->dependencies[i] : "");
            }
            for (int i = 0; i < docblock->internal_call_count; i++) {
                fprintf(output, "| `%s` | Internal Call |\n", 
                        docblock->internal_calls[i] ? docblock->internal_calls[i] : "");
            }
        } else {
            for (int i = 0; i < docblock->dependency_count; i++) {
                fprintf(output, "* Dependency: `%s`\n", 
                        docblock->dependencies[i] ? docblock->dependencies[i] : "");
            }
            for (int i = 0; i < docblock->internal_call_count; i++) {
                fprintf(output, "* Internal Call: `%s`\n", 
                        docblock->internal_calls[i] ? docblock->internal_calls[i] : "");
            }
        }
        fprintf(output, "\n");
    }
} 
