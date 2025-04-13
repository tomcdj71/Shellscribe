/**
 * @file markdown.c
 * @brief Implementation of Markdown renderer for generating documentation
 *
 * This file contains the main entry point for rendering documentation in
 * Markdown format. It handles the overall structure of the documentation,
 * including heading hierarchy, metadata, table of contents, function 
 * documentation blocks, and footer sections.
 */

#include "renderers/markdown.h"
#include "renderers/renderer_engine.h"
#include "renderers/docblock.h"
#include "renderers/style.h"
#include "renderers/table_of_content.h"
#include "renderers/github.h"
#include "utils/string.h"
#include "utils/debug.h"
#include "utils/memory.h"
#include "core/model.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Render documentation in Markdown format
 * 
 * This function is the main entry point for rendering a collection of documentation
 * blocks into a complete Markdown document. It handles the overall document structure,
 * including title, metadata sections, table of contents, individual function 
 * documentation, and footer information.
 *
 * @param docblocks Array of documentation blocks to render
 * @param count Number of documentation blocks in the array
 * @param output File stream where the rendered documentation will be written
 * @param config Configuration settings controlling the rendering behavior
 *
 * @return bool true if rendering completed successfully, false otherwise
 * 
 * @note The first block (index 0) is treated as the file-level documentation
 * @note Functions with is_internal=true are skipped in the documentation
 * @note Document sections include: title, about section (with metadata), 
 *       table of contents, function documentation, and footer
 */
bool render_markdown(const shellscribe_docblock_t *docblocks, int count, FILE *output, const shellscribe_config_t *config) {
    if (docblocks == NULL || output == NULL || config == NULL) {
        return false;
    }
    shellscribe_docblock_t *model = model_create(docblocks, count, config);
    if (model == NULL) {
        return false;
    }
    const shellscribe_docblock_t *file_metadata = &docblocks[0];
    if (file_metadata) {
        if (file_metadata->file_name != NULL) {
            const char *filename = file_metadata->file_name;
            if (file_metadata->version != NULL && config->version_placement && strcmp(config->version_placement, "filename") == 0) {
                fprintf(output, "# %s (v%s)\n\n", filename, file_metadata->version);
            } else {
                fprintf(output, "# %s\n\n", filename);
            }
        } else {
            const char *filename = strrchr(config->filename, '/');
            filename = filename ? filename + 1 : config->filename;
            fprintf(output, "# %s\n\n", filename);
        }
        bool has_about_section = file_metadata->description != NULL || file_metadata->author != NULL || 
                    file_metadata->project != NULL || file_metadata->interpreter != NULL ||
                    (file_metadata->version != NULL && config->version_placement && strcmp(config->version_placement, "about") == 0) ||
                    (file_metadata->license != NULL && config->license_placement && strcmp(config->license_placement, "about") == 0) ||
                    (file_metadata->copyright != NULL && config->copyright_placement && strcmp(config->copyright_placement, "about") == 0);
        if (has_about_section) {
            fprintf(output, "## About\n\n");
            if (file_metadata->interpreter != NULL) {
                fprintf(output, "**Interpreter:** %s\n", file_metadata->interpreter);
            }
            if (file_metadata->project != NULL) {
                fprintf(output, "**Project:** %s\n\n", file_metadata->project);
            }
            if (file_metadata->version != NULL && config->version_placement && strcmp(config->version_placement, "about") == 0) {
                fprintf(output, "**Version:** %s\n\n", file_metadata->version);
            }
            if (file_metadata->license != NULL && config->license_placement && strcmp(config->license_placement, "about") == 0) {
                fprintf(output, "**License:** %s\n\n", file_metadata->license);
            }
            if (file_metadata->copyright != NULL && config->copyright_placement && strcmp(config->copyright_placement, "about") == 0) {
                fprintf(output, "**Copyright:** %s\n\n", file_metadata->copyright);
            }
            if (file_metadata->description != NULL) {
                fprintf(output, "**Description:** %s\n\n", file_metadata->description);
            }
            if (file_metadata->author != NULL) {
                render_authors(file_metadata->author, output, config);
            }

            fprintf(output, "---\n\n");
        }
    }
    if (count > 0 && config->show_toc) {
        fprintf(output, "## Index\n\n");
        render_toc(docblocks, count, output, config);
        fprintf(output, "\n");
    }
    for (int i = 0; i < count; i++) {
        if (model[i].is_internal || model[i].function_name == NULL) {
            continue;
        }
        render_docblock(&docblocks[i], output, config);
    }
    if (file_metadata) {
        bool has_license_pre_footer = (file_metadata->license != NULL && (!config->license_placement || strcmp(config->license_placement, "pre-footer") == 0));
        bool has_copyright_pre_footer = (file_metadata->copyright != NULL && (!config->copyright_placement || strcmp(config->copyright_placement, "pre-footer") == 0));
        bool has_license_footer = (file_metadata->license != NULL && (config->license_placement && strcmp(config->license_placement, "footer") == 0));
        bool has_copyright_footer = (file_metadata->copyright != NULL && (config->copyright_placement && strcmp(config->copyright_placement, "footer") == 0));
        if (has_license_pre_footer || has_copyright_pre_footer) {
            fprintf(output, "\n---\n\n");
            if (has_license_pre_footer) {
                fprintf(output, "**License:** %s\n\n", file_metadata->license);
            }
            if (has_copyright_pre_footer) {
                fprintf(output, "**Copyright:** %s\n\n", file_metadata->copyright);
            }
        }
        if (config->footer_text != NULL || has_license_footer || has_copyright_footer) {
            fprintf(output, "\n---\n\n");
            if (has_copyright_footer) {
                fprintf(output, "**Copyright:** %s\n\n", file_metadata->copyright);
            }
            if (has_license_footer) {
                fprintf(output, "**License:** %s\n\n", file_metadata->license);
            }
            if (config->footer_text != NULL) {
                fprintf(output, "%s\n", config->footer_text);
            }
            model_free(model);
            return true;
        }
    }
    if (config->footer_text != NULL) {
        fprintf(output, "\n---\n\n");
        fprintf(output, "%s\n", config->footer_text);
    }
    model_free(model);
    
    return true;
}
