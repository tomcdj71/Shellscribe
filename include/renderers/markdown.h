/**
 * @file markdown.h
 * @brief Main Markdown renderer for shellscribe
 */

#ifndef SHELLSCRIBE_RENDERERS_MARKDOWN_RENDERER_H
#define SHELLSCRIBE_RENDERERS_MARKDOWN_RENDERER_H

#include <stdio.h>
#include <stdbool.h>
#include "parsers/types.h"
#include "utils/config.h"

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

#endif /* SHELLSCRIBE_RENDERERS_MARKDOWN_RENDERER_H */ 
