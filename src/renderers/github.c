/**
 * @file github.c
 * @brief Implementation of GitHub-specific rendering features
 *
 * This file contains functions for rendering GitHub-specific Markdown features
 * like alerts, task lists, collapsed sections, and handling GitHub usernames
 * with proper linkification.
 */

#include "renderers/github.h"
#include "utils/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Render a GitHub alert box
 * 
 * Formats and renders a GitHub-flavored Markdown alert using the blockquote-based
 * syntax: > [!TYPE]. Only certain alert types are supported by GitHub:
 * note, warning, important, and tip.
 * 
 * @param alert Pointer to the alert structure containing type and content
 * @param output File stream where the alert will be written
 * @param config Configuration settings for customizing the alert appearance
 * 
 * @note If the alert type is not recognized as one of the supported types,
 *       it defaults to "note"
 * @note Each line of the content is prefixed with the blockquote marker '>'
 */
void render_github_alert(const shellscribe_alert_t *alert, FILE *output, const shellscribe_config_t *config) {
    if (alert == NULL || output == NULL) {
        return;
    }
    const char *type = alert->type;
    const char *content = alert->content;
    const char *valid_type = "note";
    if (type != NULL) {
        if (strcmp(type, "note") == 0 || 
            strcmp(type, "warning") == 0 || 
            strcmp(type, "important") == 0 || 
            strcmp(type, "tip") == 0) {
            valid_type = type;
        }
    }
    fprintf(output, "> [!%s]\n", valid_type);
    if (content != NULL) {
        char *message_copy = string_duplicate(content);
        char *line = strtok(message_copy, "\n");
        while (line != NULL) {
            fprintf(output, "> %s\n", line);
            line = strtok(NULL, "\n");
        }
        free(message_copy);
    }
    fprintf(output, "\n");
}

/**
 * @brief Render a GitHub task list (checkbox list)
 * 
 * Creates a markdown list with checkboxes (task list) from an array of strings.
 * Each item is rendered as an unchecked checkbox item in the form "- [ ] item".
 * 
 * @param items Array of strings to be rendered as task list items
 * @param count Number of items in the array
 * @param output File stream where the task list will be written
 * 
 * @note All checkboxes are rendered as unchecked ([ ])
 * @note A blank line is added after the task list
 */
void render_github_task_list(const char **items, int count, FILE *output) {
    if (items == NULL || output == NULL || count <= 0) {
        return;
    }
    for (int i = 0; i < count; i++) {
        if (items[i] != NULL) {
            fprintf(output, "- [ ] %s\n", items[i]);
        }
    }
    fprintf(output, "\n");
}

/**
 * @brief Render a GitHub collapsible section using HTML details/summary tags
 * 
 * Creates a collapsible section using GitHub-supported HTML tags that can be
 * expanded or collapsed by the user clicking on the summary.
 * 
 * @param summary Text to display as the clickable header of the section
 * @param content Content to display when the section is expanded
 * @param output File stream where the collapsible section will be written
 * 
 * @note The section is initially rendered in the collapsed state
 * @note GitHub Markdown allows HTML tags like details/summary in Markdown
 */
void render_github_collapsed_section(const char *summary, const char *content, FILE *output) {
    if (summary == NULL || content == NULL || output == NULL) {
        return;
    }
    fprintf(output, "<details>\n");
    fprintf(output, "<summary>%s</summary>\n\n", summary);
    fprintf(output, "%s\n", content);
    fprintf(output, "</details>\n\n");
}

/**
 * @brief Render all alerts in a documentation block
 * 
 * Processes all alert entries in a documentation block and renders them
 * using the GitHub alert format if alert display is enabled in the config.
 * 
 * @param docblock Documentation block containing alerts to render
 * @param output File stream where the alerts will be written
 * @param config Configuration settings that control alert rendering
 * 
 * @note If config->show_alerts is false, this function does nothing
 */
void render_alerts(const shellscribe_docblock_t *docblock, FILE *output, const shellscribe_config_t *config) {
    if (docblock == NULL || output == NULL || config == NULL) {
        return;
    }
    if (!config->show_alerts) {
        return;
    }
    for (int i = 0; i < docblock->alert_count; i++) {
        render_github_alert(&docblock->alerts[i], output, config);
    }
}

/**
 * @brief Render author information with optional GitHub profile links
 * 
 * Processes and formats author information from a comma-separated string.
 * If GitHub usernames are included in parentheses and config->linkify_usernames
 * is enabled, they are converted to GitHub profile links.
 * 
 * @param author_string Comma-separated string of author names, optionally with
 *                      GitHub usernames in parentheses like "Name (@username)"
 * @param output File stream where the author information will be written
 * @param config Configuration settings that control author rendering
 * 
 * @note GitHub usernames are recognized in the format "Name (@username)" or "Name (username)"
 * @note When linkified, usernames become clickable links to GitHub profiles
 */
void render_authors(const char *author_string, FILE *output, const shellscribe_config_t *config) {
    if (author_string == NULL || output == NULL) {
        return;
    }
    fprintf(output, "**Authors:** ");
    char *author_copy = string_duplicate(author_string);
    char *saveptr = NULL;
    char *token = strtok_r(author_copy, ",", &saveptr);
    bool first = true;
    while (token != NULL) {
        while (*token && isspace((unsigned char)*token)) {
            token++;
        }
        if (!first) {
            fprintf(output, ", ");
        }
        char *github_start = strstr(token, "(@");
        if (!github_start) {
            github_start = strstr(token, "(");
        }
        if (github_start && config && config->linkify_usernames) {
            github_start++;
            if (*github_start == '@') {
                github_start++;
            }
            char *github_end = strchr(github_start, ')');
            if (github_end) {
                size_t name_len = github_start - token - 1;
                while (name_len > 0 && isspace((unsigned char)token[name_len-1])) {
                    name_len--;
                }
                char *display_name = (char *)malloc(name_len + 1);
                if (display_name) {
                    strncpy(display_name, token, name_len);
                    display_name[name_len] = '\0';
                    fprintf(output, "%s", display_name);
                    free(display_name);
                } else {
                    fprintf(output, "%.*s", (int)name_len, token);
                }
                size_t username_len = github_end - github_start;
                char *username = (char *)malloc(username_len + 1);
                if (username) {
                    strncpy(username, github_start, username_len);
                    username[username_len] = '\0';
                    fprintf(output, "  [@%s](https://github.com/%s)", username, username);
                    free(username);
                }
            } else {
                fprintf(output, "%s", token);
            }
        } else {
            fprintf(output, "%s", token);
        }
        first = false;
        token = strtok_r(NULL, ",", &saveptr);
    }
    fprintf(output, "\n\n");
    free(author_copy);
} 
