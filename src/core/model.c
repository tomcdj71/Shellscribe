/**
 * @file model.c
 * @brief Implementation of the reference-based model for efficient memory usage
 *
 * This file provides functions for creating and managing lightweight reference models
 * of documentation blocks. Rather than duplicating all data from original documentation
 * blocks, these models maintain references to the original data, which significantly
 * reduces memory usage when working with large documentation sets. This approach is 
 * particularly useful for search and display operations that don't require modifying
 * the original data.
 */

#include "core/model.h"
#include "utils/string.h"
#include "utils/debug.h"
#include "utils/memory.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * @brief Maximum number of examples that can be extracted from a block
 *
 * This constant defines the upper limit on the number of examples that can be
 * extracted from a single documentation block. This limit helps prevent memory
 * allocation issues when processing documentation with many examples.
 */
#define MAX_EXAMPLES 10

/**
 * @brief Create a lightweight model from an array of documentation blocks
 *
 * This function creates a new array of docblocks that references the data in
 * the original array, avoiding the need to duplicate all the data. It creates
 * a memory-efficient representation that can be used for display, search,
 * and reference operations.
 *
 * @param docblocks Original array of documentation blocks to reference
 * @param count Number of blocks in the array
 * @param config Pointer to the configuration for debugging output
 * 
 * @return shellscribe_docblock_t* New array of documentation blocks containing references 
 *         to the original data, or NULL if an error occurred (e.g., memory allocation failure
 *         or invalid parameters)
 *
 * @note The returned model must be freed using model_free() when no longer needed
 * @note This function skips documentation blocks marked with the @skip annotation
 * @note The model only references function names, descriptions, and internal status;
 *       it does not duplicate or reference other fields
 * @note If debugging is enabled in the configuration, this function logs progress information
 * 
 * @see model_free
 */
shellscribe_docblock_t *model_create(const shellscribe_docblock_t *docblocks, int count, const shellscribe_config_t *config) {
    if (docblocks == NULL || count <= 0) {
        return NULL;
    }
    shellscribe_docblock_t *model = shell_calloc(count, sizeof(shellscribe_docblock_t));
    if (model == NULL) {
        return NULL;
    }
    debug_message(config, "Creating reference model: %d blocks\n", count);
    for (int i = 0; i < count; i++) {
        if (docblocks[i].is_skipped) {
            debug_message(config, "Skipping docblock %d (marked with @skip)\n", i);
            model[i].is_skipped = true;
            continue;
        }
        model[i].function_name = docblocks[i].function_name;
        model[i].function_description = docblocks[i].function_description;
        model[i].is_internal = docblocks[i].is_internal;
        debug_message(config, "Model block %d: %s\n", i, model[i].function_name ? model[i].function_name : "NULL");
    }
    
    debug_message(config, "Model created successfully\n");
    return model;
}

/**
 * @brief Free a model created by model_create
 *
 * This function frees only the model array itself, not the referenced data.
 * Since the model contains references rather than owned data, this function
 * only deallocates the array that holds these references.
 *
 * @param model The model to free, created by model_create()
 *
 * @note This function safely handles NULL input by doing nothing
 * @note This function does not free the original data that the model references
 * @note After calling this function, any references to the model are invalid
 * 
 * @see model_create
 */
void model_free(shellscribe_docblock_t *model) {
    if (model != NULL) {
        void *ptr = model;
        shell_free(&ptr);
    }
}

/**
 * @brief Get the examples from a docblock
 *
 * This function extracts examples from a docblock and returns them as an array of strings.
 * It parses the example content, which may contain multiple examples separated by blank lines,
 * and returns each example as a separate string in the array.
 *
 * @param docblock The docblock to extract examples from
 * @param count Pointer to store the number of examples found
 * 
 * @return char** Array of example strings (NULL-terminated), or NULL if no examples
 *         were found or if an error occurred
 *
 * @note The returned array and all strings it contains must be freed by the caller
 * @note The maximum number of examples that can be extracted is defined by MAX_EXAMPLES
 * @note Examples are separated by double newlines (\n\n) in the docblock's example field
 * @note If only one example is found, it is returned as a single-element array
 * @note If count is NULL or docblock doesn't contain examples, the function
 *       sets *count to 0 and returns NULL
 * 
 * @see MAX_EXAMPLES
 */
char **model_get_examples(const shellscribe_docblock_t *docblock, int *count) {
    if (docblock == NULL || docblock->example == NULL || count == NULL) {
        *count = 0;
        return NULL;
    }
    char **examples = shell_malloc(MAX_EXAMPLES * sizeof(char*));
    if (examples == NULL) {
        *count = 0;
        return NULL;
    }
    for (int i = 0; i < MAX_EXAMPLES; i++) {
        examples[i] = NULL;
    }
    char *example_copy = string_duplicate(docblock->example);
    if (example_copy == NULL) {
        void *ptr = examples;
        shell_free(&ptr);
        *count = 0;
        return NULL;
    }
    if (strstr(example_copy, "\n\n") != NULL) {
        char *saveptr = NULL;
        char *block = strtok_r(example_copy, "\n\n", &saveptr);
        int block_count = 0;
        while (block != NULL && block_count < MAX_EXAMPLES) {
            if (strlen(block) > 0) {
                examples[block_count] = string_duplicate(block);
                block_count++;
            }
            block = strtok_r(NULL, "\n\n", &saveptr);
        }
        
        *count = block_count;
    } else {
        examples[0] = string_duplicate(example_copy);
        *count = 1;
    }
    
    void *ptr = example_copy;
    shell_free(&ptr);
    return examples;
}

/**
 * @brief Check if a docblock has multiple examples
 *
 * This function determines whether a docblock contains multiple examples,
 * which is useful when deciding how to format or display the examples.
 *
 * @param docblock The docblock to check for multiple examples
 * 
 * @return bool true if the docblock contains multiple examples (separated by
 *              double newlines), false otherwise or if docblock is NULL or
 *              does not contain any examples
 *
 * @note Examples are considered multiple if they are separated by blank lines (\n\n)
 * @note This function only checks for the presence of separators and does not
 *       extract or count the actual examples
 * 
 * @see model_get_examples
 */
bool model_has_multiple_examples(const shellscribe_docblock_t *docblock) {
    if (docblock == NULL || docblock->example == NULL) {
        return false;
    }
    
    return (strstr(docblock->example, "\n\n") != NULL);
}

/**
 * @brief Get the metadata of a file from the model
 *
 * This function retrieves the metadata of a file from the model, which is
 * always stored in the first element of the model array. File metadata includes
 * information like file name, description, version, author, etc.
 *
 * @param model The model to retrieve metadata from, created by model_create()
 * 
 * @return const shellscribe_docblock_t* Pointer to the file metadata docblock,
 *         or NULL if the model is NULL
 *
 * @note This function assumes that the first element of the model array always
 *       contains the file-level metadata
 * @note The returned pointer is a reference to the data within the model and
 *       should not be freed separately
 * @note The returned pointer becomes invalid when the model is freed
 * 
 * @see model_create
 * @see model_free
 */
const shellscribe_docblock_t *model_get_file_metadata(const shellscribe_docblock_t *model) {
    return (model != NULL) ? &model[0] : NULL;
} 
