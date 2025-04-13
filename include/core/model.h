/**
 * @file model.h
 * @brief API for managing the documentation model
 * 
 * This file defines the API for working with the documentation model,
 * providing clear separation between parsing and rendering.
 */

#ifndef SHELLSCRIBE_CORE_MODEL_H
#define SHELLSCRIBE_CORE_MODEL_H

#include "parsers/types.h"
#include "utils/config.h"

/**
 * @brief Initialize the documentation model from parsed blocks
 * 
 * Creates a model that can be used by renderers from the parsed docblocks.
 * 
 * @param docblocks Array of parsed documentation blocks
 * @param count Number of blocks in the array
 * @param config Configuration settings
 * @return A model that can be passed to renderers
 */
shellscribe_docblock_t *model_create(const shellscribe_docblock_t *docblocks, int count, const shellscribe_config_t *config);

/**
 * @brief Free resources associated with a model
 * 
 * @param model The model to be freed
 */
void model_free(shellscribe_docblock_t *model);

/**
 * @brief Get examples from a documentation block
 * 
 * @param docblock The documentation block
 * @param count Pointer to store the number of examples
 * @return Array of example strings
 */
char **model_get_examples(const shellscribe_docblock_t *docblock, int *count);

/**
 * @brief Does the documentation block contain multiple examples?
 * 
 * @param docblock The documentation block to check
 * @return true if multiple examples exist, false otherwise
 */
bool model_has_multiple_examples(const shellscribe_docblock_t *docblock);

/**
 * @brief Get file metadata from the model
 * 
 * @param model The documentation model
 * @return Pointer to the file metadata block
 */
const shellscribe_docblock_t *model_get_file_metadata(const shellscribe_docblock_t *model);

#endif /* SHELLSCRIBE_CORE_MODEL_H */ 
