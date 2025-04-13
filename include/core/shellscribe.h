/**
* @file shellscribe.h
* @brief Main shellscribe declarations
*/

#ifndef SHELLSCRIBE_H
#define SHELLSCRIBE_H

#include <stdio.h>
#include <stdbool.h>

#include "utils/config.h"
#include "parsers/types.h"

/**
 * @brief Parse a shell script and return the documentation blocks
 * 
 * @param file_path Path to the shell script file 
 * @param block_count Output parameter to store the number of blocks
 * @param config Configuration options
 * @return shellscribe_docblock_t* Array of documentation blocks, or NULL on error
 */
shellscribe_docblock_t* parse_shell_script(const char *file_path, int *block_count, const shellscribe_config_t *config);

#endif /* SHELLSCRIBE_H */
