/**
 * @file style.c
 * @brief Utility functions for rendering styled text
 */

#include "renderers/style.h"
#include "utils/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Render styled text based on the provided type and configuration
 * 
 * This function applies formatting to text based on the style configuration.
 * It takes a style type identifier (e.g., "h1", "strong", "code"), looks up the
 * appropriate formatting tags in the configuration, and wraps the input text
 * with those tags.
 * 
 * @param type The type of style to apply (e.g., "h1", "strong", "code")
 * @param text The text content to be styled
 * @param config The configuration containing style definitions
 * 
 * @return char* A newly allocated string containing the styled text, or a copy
 *               of the original text if styling can't be applied. Returns an empty
 *               string if text is NULL. Must be freed by the caller.
 * 
 * @note If the style type is not recognized, the function returns an unmodified
 *       copy of the input text.
 */
char *render_styled_text(const char *type, const char *text, const shellscribe_config_t *config) {
    if (text == NULL || type == NULL || config == NULL) {
        return string_duplicate(text != NULL ? text : "");
    }
    
    char *result = NULL;
    const char *from = NULL;
    const char *to = NULL;
    
    struct {
        const char *type;
        char * const *from;
        char * const *to;
    } style_map[] = {
        {"h1", &config->style.h1_from, &config->style.h1_to},
        {"h2", &config->style.h2_from, &config->style.h2_to},
        {"h3", &config->style.h3_from, &config->style.h3_to},
        {"h4", &config->style.h4_from, &config->style.h4_to},
        {"strong", &config->style.strong_from, &config->style.strong_to},
        {"code", &config->style.code_from, &config->style.code_to},
        {"i", &config->style.i_from, &config->style.i_to},
        {"dt", &config->style.dt_from, &config->style.dt_to},
        {"dd", &config->style.dd_from, &config->style.dd_to},
        {"argN", &config->style.argN_from, &config->style.argN_to},
        {"arg@", &config->style.arg_at_from, &config->style.arg_at_to},
        {"anchor", &config->style.anchor_from, &config->style.anchor_to},
        {NULL, NULL, NULL}
    };

    for (int i = 0; style_map[i].type != NULL; i++) {
        if (strcmp(type, style_map[i].type) == 0) {
            from = *style_map[i].from;
            to = *style_map[i].to;
            break;
        }
    }
    
    if (from != NULL && to != NULL) {
        size_t len = strlen(from) + strlen(text) + strlen(to) + 1;
        result = (char *)malloc(len);
        if (result != NULL) {
            sprintf(result, "%s%s%s", from, text, to);
        }
    } else {
        result = string_duplicate(text);
    }
    
    return result;
} 
