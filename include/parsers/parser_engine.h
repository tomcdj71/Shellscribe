/**
 * @file parser_engine.h
 * @brief Main parser for shellscribe
 */

#ifndef SHELLSCRIBE_PARSER_ENGINE_H
#define SHELLSCRIBE_PARSER_ENGINE_H

#include <stdio.h>
#include <stdbool.h>
#include "utils/config.h"
#include "parsers/types.h"
#include "parsers/tag.h"
#include "parsers/function.h"
#include "parsers/metadata.h"
#include "parsers/annotation.h"

/**
 * @brief Parse a shell script to extract documentation
 * 
 * @param file_path Path to the shell script file
 * @param config Configuration options
 * @param docblocks Array to store the extracted documentation blocks
 * @param max_blocks Maximum number of blocks to extract
 * @return Number of blocks extracted or -1 on error
 */
int parse_shell_file(const char *file_path, const shellscribe_config_t *config,
    shellscribe_docblock_t *docblocks, int max_blocks);

#endif /* SHELLSCRIBE_PARSER_ENGINE_H */ 
