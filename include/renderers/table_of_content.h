/**
 * @file toc.h
 * @brief Table of Contents renderer for shellscribe
 */

#ifndef SHELLSCRIBE_RENDERERS_TOC_RENDERER_H
#define SHELLSCRIBE_RENDERERS_TOC_RENDERER_H

#include <stdio.h>
#include "parsers/types.h"
#include "utils/config.h"

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
 * @brief Generates a link for the table of contents
 *
 * @param title Link title
 * @param config Pointer to the configuration
 * @return char* Newly allocated string containing the link (to be freed by the caller)
 */
char *render_toc_link(const char *title, const shellscribe_config_t *config);

#endif /* SHELLSCRIBE_RENDERERS_TOC_RENDERER_H */ 
