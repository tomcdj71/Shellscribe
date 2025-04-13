/**
 * @file renderer_engine.h
 * @brief Main renderer header for shellscribe
 */

#ifndef SHELLSCRIBE_RENDERER_H
#define SHELLSCRIBE_RENDERER_H

#include <stdio.h>
#include <stdbool.h>

#include "parsers/types.h"
#include "utils/config.h"
#include "renderers/markdown.h"
#include "renderers/table_of_content.h"
#include "renderers/docblock.h"
#include "renderers/style.h"
#include "renderers/github.h"

/**
 * @brief Renders documentation in the appropriate format
 *
 * @param docblocks Array of documentation blocks
 * @param count Number of blocks in the array
 * @param output Output stream to write the documentation to
 * @param config Pointer to the configuration
 * @return bool True if rendering was successful, false otherwise
 */
bool render_documentation(const shellscribe_docblock_t *docblocks, int count,
                        FILE *output, const shellscribe_config_t *config);

/**
 * @brief Generates Markdown documentation for an array of documentation blocks
 *
 * @param docblocks Array of documentation blocks
 * @param count Number of blocks in the array
 * @param output Output stream to write the documentation to
 * @param config Pointer to the configuration
 * @return bool True if rendering was successful, false otherwise
 */
bool render_markdown(const shellscribe_docblock_t *docblocks, int count, FILE *output, const shellscribe_config_t *config);

/**
* @brief Generates the table of contents for the documentation blocks
*
* @param docblocks Array of documentation blocks
* @param count Number of blocks in the array
* @param output Output stream to write the table of contents to
* @param config Pointer to the configuration
*/
void render_toc(const shellscribe_docblock_t *docblocks, int count, FILE *output, const shellscribe_config_t *config);

/**
* @brief Generates the documentation for a specific block
*
* @param docblock Documentation block to render
* @param output Output stream to write the documentation to
* @param config Pointer to the configuration
*/
void render_docblock(const shellscribe_docblock_t *docblock, FILE *output, const shellscribe_config_t *config);

/**
* @brief Creates an anchor link for a function name
*
* @param function_name Function name to create an anchor for
* @return char* Newly allocated string containing the anchor (to be freed by the caller)
*/
char *create_anchor_link(const char *function_name);

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
* @brief Renders author information with GitHub links
*
* @param author_string Author string to parse
* @param output Output stream to write the formatted authors to
* @param config Pointer to the configuration
*/
void render_authors(const char *author_string, FILE *output, const shellscribe_config_t *config);

#endif /* SHELLSCRIBE_RENDERER_H */
