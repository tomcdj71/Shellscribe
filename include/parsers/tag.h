/**
 * @file tag.h
 * @brief Tag extraction and processing for shellscribe
 */

#ifndef SHELLSCRIBE_PARSERS_TAG_H
#define SHELLSCRIBE_PARSERS_TAG_H

#include <stdbool.h>
#include "parsers/types.h"
#include "utils/config.h"

/**
 * @brief Structure representing a parsed tag
 */
typedef struct {
    char *name;        // Tag name (without @ prefix)
    char *content;     // Tag content
} tag_t;

/**
 * @brief Check if a line contains a tag
 * 
 * @param line Line to check
 * @return bool True if the line contains a tag
 */
extern bool is_tag_line(const char *line);

/**
 * @brief Extract tag name from a line
 * 
 * @param line Line containing a tag
 * @return char* Newly allocated string containing the tag name (to be freed by the caller), NULL on error
 */
extern char *extract_tag_name(const char *line);

/**
 * @brief Extract tag content from a line
 * 
 * @param line Line containing a tag
 * @return char* Newly allocated string containing the tag content (to be freed by the caller), NULL on error
 */
extern char *extract_tag_content(const char *line);

/**
 * @brief Process a tag and update a documentation block
 * 
 * @param docblock Documentation block to update
 * @param tag Tag name (without @ prefix)
 * @param content Tag content
 * @param config Configuration options
 * @return bool True if the tag was processed successfully
 */
bool process_tag(shellscribe_docblock_t *docblock, const char *tag, const char *content, const shellscribe_config_t *config);

/**
 * @brief Collect continued content from multi-line comments
 * 
 * @param file Input file
 * @param initial_content Initial content from the first line
 * @param config Configuration options
 * @return char* Newly allocated string containing the collected content (to be freed by the caller)
 */
char *collect_continued_content(FILE *file, const char *initial_content, const shellscribe_config_t *config);

#endif /* SHELLSCRIBE_PARSERS_TAG_H */ 
