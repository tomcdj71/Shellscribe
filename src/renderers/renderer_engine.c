/**
 * @file renderer_engine.c
 * @brief Main rendering logic for shellscribe document generation
 * 
 * This file provides the rendering engine which delegates to format-specific 
 * renderers to transform parsed documentation blocks into formatted documentation.
 * Currently supports Markdown output, but is designed to be extended to other formats.
 * The engine implements the basic rendering pipeline and common utility functions
 * that can be shared across multiple output formats.
 */

#include "renderers/renderer_engine.h"
#include "renderers/markdown.h"
#include "utils/debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Render documentation from parsed documentation blocks
 * 
 * Processes an array of documentation blocks and generates properly formatted
 * documentation based on the configured output format. Currently supports
 * Markdown but is designed to be extended to other formats like HTML, AsciiDoc,
 * or plain text in the future.
 * 
 * This function is the primary entry point for the rendering process and
 * delegates to format-specific renderers based on the configuration.
 * 
 * @param docblocks Array of documentation blocks to render. Must not be NULL.
 *                  The array should be pre-populated with parsed documentation.
 * @param count Number of documentation blocks in the array. Must be greater than 0.
 * @param output File handle where the rendered documentation will be written.
 *               Must be opened for writing before calling this function.
 * @param config Configuration settings controlling the rendering behavior.
 *               Includes settings for styling, formatting preferences, and
 *               output customization options.
 * 
 * @return bool true if rendering completed successfully, false if any required
 *              parameter is NULL, count is less than or equal to 0, or if the
 *              format-specific renderer encounters an error
 * 
 * @note The first block (index 0) is typically the file-level documentation
 *       and is treated specially by most renderers
 * @note This function currently only supports Markdown output, but the architecture
 *       is designed for future extension to other formats
 * 
 * @see render_markdown
 */
bool render_documentation(const shellscribe_docblock_t *docblocks, int count, FILE *output, const shellscribe_config_t *config) {
    if (docblocks == NULL || output == NULL || config == NULL || count <= 0) {
        return false;
    }
    return render_markdown(docblocks, count, output, config);
}

/**
 * @brief Create an anchor link from a function name
 * 
 * Generates an anchor link that can be used in the documentation to create
 * navigation links to specific functions. This is especially useful for
 * table of contents and cross-referencing within the generated documentation.
 * 
 * The anchor format follows standard conventions for HTML anchors and Markdown
 * heading IDs, making it compatible with most documentation systems and viewers.
 * 
 * @param function_name Name of the function to create an anchor for. This should
 *                      be the exact function name as it appears in the code.
 * 
 * @return char* Newly allocated string containing the anchor name. The caller must
 *               free this memory when it's no longer needed to prevent memory leaks.
 *               Returns NULL if the input is NULL or if memory allocation fails.
 * 
 * @note Currently returns a simple copy of the function name, but can be 
 *       extended to handle special characters, spaces, or other formatting
 *       requirements for specific documentation systems
 * @note Future extensions could include converting spaces to hyphens, handling
 *       special characters, or ensuring uniqueness across the document
 * @note Potential memory allocation failures are handled by returning NULL
 */
char *create_anchor_link(const char *function_name) {
    if (function_name == NULL) {
        return NULL;
    }
    size_t len = strlen(function_name);
    char *anchor = (char *)malloc(len + 1);
    if (anchor == NULL) {
        return NULL;
    }
    strcpy(anchor, function_name);
    
    return anchor;
} 
