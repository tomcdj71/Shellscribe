/**
 * @file toc.c
 * @brief Table of Contents renderer for Shellscribe
 *
 * This file provides functionality for generating a table of contents
 * for the documentation, listing all functions with links to their
 * corresponding documentation sections.
 */

#include "renderers/table_of_content.h"
#include "renderers/renderer_engine.h"
#include "utils/string.h"
#include "utils/debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Renders a table of contents for all documented functions
 * 
 * This function generates a Markdown-formatted table of contents based on the
 * function names and brief descriptions found in the documentation blocks.
 * Each entry in the table of contents includes a link to the corresponding
 * function section in the documentation.
 * 
 * @param docblocks Array of documentation blocks to include in the TOC
 * @param count Number of documentation blocks in the array
 * @param output File stream where the TOC will be written
 * @param config Configuration settings for the rendering process
 * 
 * @note The function skips documentation blocks without a function name
 * @note If config->show_toc is false, this function does nothing
 * @note Each function's brief description is included in the TOC if available
 */
void render_toc(const shellscribe_docblock_t *docblocks, int count, FILE *output, const shellscribe_config_t *config) {
    if (docblocks == NULL || output == NULL || config == NULL || count <= 0) {
        return;
    }
    if (!config->show_toc) {
        return;
    }
    int function_count = 0;
    for (int i = 0; i < count; i++) {
        if (docblocks[i].function_name != NULL) {
            function_count++;
        }
    }
    if (function_count <= 0) {
        return;
    }
    for (int i = 0; i < count; i++) {
        if (docblocks[i].function_name != NULL) {
            char *anchor = create_anchor_link(docblocks[i].function_name);
            fprintf(output, "* [%s](#%s)", 
                    docblocks[i].function_name, 
                    anchor ? anchor : "");
            if (docblocks[i].function_brief != NULL) {
                fprintf(output, " - %s", docblocks[i].function_brief);
            }
            fprintf(output, "\n");
            free(anchor);
        }
    }
    
    fprintf(output, "\n\n");
} 
