/**
 * @file docblock.h
 * @brief Documentation block renderer for shellscribe
 */

#ifndef SHELLSCRIBE_RENDERERS_DOCBLOCK_RENDERER_H
#define SHELLSCRIBE_RENDERERS_DOCBLOCK_RENDERER_H

#include <stdio.h>
#include "parsers/types.h"
#include "utils/config.h"

/**
 * @brief Generates the documentation for a specific block
 *
 * @param docblock Documentation block to render
 * @param output Output stream to write the documentation to
 * @param config Pointer to the configuration
 */
void render_docblock(const shellscribe_docblock_t *docblock, FILE *output, const shellscribe_config_t *config);

/**
 * @brief Renders the function name section
 *
 * @param docblock Documentation block to render
 * @param output Output stream to write to
 * @param config Pointer to the configuration
 */
void render_function_name(const shellscribe_docblock_t *docblock, FILE *output, const shellscribe_config_t *config);

/**
 * @brief Renders the function description section
 *
 * @param docblock Documentation block to render
 * @param output Output stream to write to
 * @param config Pointer to the configuration
 */
void render_function_description(const shellscribe_docblock_t *docblock, FILE *output, const shellscribe_config_t *config);

/**
 * @brief Renders the examples section
 *
 * @param docblock Documentation block to render
 * @param output Output stream to write to
 * @param config Pointer to the configuration
 */
void render_examples(const shellscribe_docblock_t *docblock, FILE *output, const shellscribe_config_t *config);

/**
 * @brief Renders the arguments section
 *
 * @param docblock Documentation block to render
 * @param output Output stream to write to
 * @param config Pointer to the configuration
 */
void render_arguments(const shellscribe_docblock_t *docblock, FILE *output, const shellscribe_config_t *config);

/**
 * @brief Renders the dependencies section
 *
 * @param docblock Documentation block to render
 * @param output Output stream to write to
 * @param config Pointer to the configuration
 */
void render_dependencies(const shellscribe_docblock_t *docblock, FILE *output, const shellscribe_config_t *config);

/**
 * @brief Renders the options section
 *
 * @param docblock Documentation block to render
 * @param output Output stream to write to
 * @param config Pointer to the configuration
 */
void render_options(const shellscribe_docblock_t *docblock, FILE *output, const shellscribe_config_t *config);

/**
 * @brief Renders the exit codes section
 *
 * @param docblock Documentation block to render
 * @param output Output stream to write to
 * @param config Pointer to the configuration
 */
void render_exit_codes(const shellscribe_docblock_t *docblock, FILE *output, const shellscribe_config_t *config);

/**
 * @brief Renders the stdout section
 *
 * @param docblock Documentation block to render
 * @param output Output stream to write to
 * @param config Pointer to the configuration
 */
void render_stdout(const shellscribe_docblock_t *docblock, FILE *output, const shellscribe_config_t *config);

/**
 * @brief Renders the stderr section
 *
 * @param docblock Documentation block to render
 * @param output Output stream to write to
 * @param config Pointer to the configuration
 */
void render_stderr(const shellscribe_docblock_t *docblock, FILE *output, const shellscribe_config_t *config);

/**
 * @brief Renders the see also section
 *
 * @param docblock Documentation block to render
 * @param output Output stream to write to
 * @param config Pointer to the configuration
 */
void render_see_also(const shellscribe_docblock_t *docblock, FILE *output, const shellscribe_config_t *config);

#endif /* SHELLSCRIBE_RENDERERS_DOCBLOCK_RENDERER_H */ 
