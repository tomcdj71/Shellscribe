/**
 * @file github.h
 * @brief GitHub-specific rendering for shellscribe
 */

#ifndef SHELLSCRIBE_RENDERERS_GITHUB_RENDERER_H
#define SHELLSCRIBE_RENDERERS_GITHUB_RENDERER_H

#include <stdio.h>
#include "parsers/types.h"
#include "utils/config.h"

/**
 * @brief Renders GitHub alert blocks
 *
 * @param alert Alert to render
 * @param output Output stream to write to
 * @param config Pointer to the configuration
 */
void render_github_alert(const shellscribe_alert_t *alert, FILE *output, const shellscribe_config_t *config);

/**
 * @brief Format author name with GitHub profile link
 *
 * @param author_text Original author text
 * @return char* Newly allocated string containing the formatted author text (to be freed by the caller)
 */
char *format_author_with_github_link(const char *author_text);

/**
 * @brief Renders author information with GitHub links
 *
 * @param author_string Author string to parse (format: "Name (@username)")
 * @param output Output stream to write the formatted authors to
 * @param config Pointer to the configuration
 */
void render_authors(const char *author_string, FILE *output, const shellscribe_config_t *config);

/**
 * @brief Renders all alerts in a documentation block
 *
 * @param docblock Documentation block containing alerts
 * @param output Output stream to write to
 * @param config Pointer to the configuration
 */
void render_alerts(const shellscribe_docblock_t *docblock, FILE *output, const shellscribe_config_t *config);

#endif /* SHELLSCRIBE_RENDERERS_GITHUB_RENDERER_H */ 
