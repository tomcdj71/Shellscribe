/**
 * @file style.h
 * @brief Style utilities for shellscribe
 */

#ifndef SHELLSCRIBE_RENDERERS_STYLE_UTILS_H
#define SHELLSCRIBE_RENDERERS_STYLE_UTILS_H

#include "utils/config.h"

/**
 * @brief Transforms text according to a specific style
 *
 * @param type Style type (h1, h2, strong, etc.)
 * @param text Text to transform
 * @param config Pointer to the configuration
 * @return char* Newly allocated string containing the transformed text (to be freed by the caller)
 */
char *render_styled_text(const char *type, const char *text, const shellscribe_config_t *config);

/**
 * @brief Wraps code in code block markers
 *
 * @param text Code to wrap
 * @param language Language for syntax highlighting (e.g., "bash")
 * @param config Pointer to the configuration
 * @return char* Newly allocated string containing the code block (to be freed by the caller)
 */
char *render_code_block(const char *text, const char *language, const shellscribe_config_t *config);

/**
 * @brief Creates a Markdown link
 *
 * @param text Link text
 * @param url Link URL
 * @param config Pointer to the configuration
 * @return char* Newly allocated string containing the link (to be freed by the caller)
 */
char *render_link(const char *text, const char *url, const shellscribe_config_t *config);

/**
 * @brief Formats a list item
 *
 * @param text Item text
 * @param config Pointer to the configuration
 * @return char* Newly allocated string containing the list item (to be freed by the caller)
 */
char *render_list_item(const char *text, const shellscribe_config_t *config);

#endif /* SHELLSCRIBE_RENDERERS_STYLE_UTILS_H */ 
