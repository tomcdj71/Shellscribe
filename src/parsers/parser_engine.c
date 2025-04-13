/**
 * @file parser_engine.c
 * @brief Implementation of the core parser engine for shell script documentation
 * 
 * This file contains the main parsing logic that processes shell scripts,
 * extracts documentation comments, and constructs documentation blocks.
 * It coordinates the various specialized parsers for different documentation tags.
 */

#include "parsers/parser_engine.h"
#include "parsers/types.h"
#include "parsers/state.h"
#include "parsers/tag.h"
#include "parsers/function.h"
#include "parsers/metadata.h"
#include "parsers/annotation.h"
#include "parsers/example.h"
#include "parsers/argument.h"
#include "parsers/return.h"
#include "parsers/shellcheck.h"
#include "parsers/description.h"
#include "parsers/section.h"
#include "parsers/option.h"
#include "parsers/variable.h"
#include "parsers/exitcode.h"
#include "parsers/io.h"
#include "parsers/reference.h"
#include "parsers/internal.h"
#include "parsers/deprecated.h"
#include "parsers/alias.h"
#include "parsers/alert.h"

#include "utils/string.h"
#include "utils/debug.h"
#include "utils/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Maximum number of documentation blocks to parse in a file
 */
#define MAX_DOC_BLOCKS 100

/**
 * @brief Maximum length of a line in a shell script
 */
#define MAX_LINE_LENGTH 4096

/**
 * @brief Prefix that identifies a documentation tag in comments
 */
#define TAG_PREFIX "# @"

static bool process_tag_internal(parser_state_t *state, const char *tag, const char *content);
static char *collect_continued_comment_content(parser_state_t *state, const char *initial_content);
extern bool is_comment_line(const char *line);
extern bool is_tag_line(const char *line);
extern char *extract_tag_name(const char *line);
extern char *extract_tag_content(const char *line);
static bool process_tag_internal(parser_state_t *state, const char *tag, const char *content);
extern bool is_function_declaration(const char *line);
extern char *extract_function_name(const char *line);
static char *collect_continued_comment_content(parser_state_t *state, const char *initial_content);
extern bool is_special_annotation(const char *line);
extern bool extract_shebang(const char *line, shellscribe_docblock_t *docblock);
extern bool process_warning_tag(shellscribe_docblock_t *docblock, const char *content);
extern bool process_dependency_tag(shellscribe_docblock_t *docblock, const char *content);
extern bool process_internal_call_tag(shellscribe_docblock_t *docblock, const char *content);
extern bool process_requires_tag(shellscribe_docblock_t *docblock, const char *content);
extern bool process_used_by_tag(shellscribe_docblock_t *docblock, const char *content);
extern bool process_calls_tag(shellscribe_docblock_t *docblock, const char *content);
extern bool process_provides_tag(shellscribe_docblock_t *docblock, const char *content);
bool is_file_level_tag(const char *tag);
bool is_file_level_description(const char *tag, int line_number);
bool process_file_metadata_tag(shellscribe_docblock_t *block, const char *tag, const char *content);

/**
 * @brief Extract and process the interpreter shebang line from a shell script
 * 
 * Examines a line to determine if it's a shebang (#!) line, and if so,
 * extracts the interpreter path and stores it in the documentation block.
 * 
 * @param line The line to examine for a shebang
 * @param docblock The documentation block to store the interpreter in
 * 
 * @return bool true if a valid shebang was found and processed,
 *              false if the line doesn't contain a shebang or is invalid
 */
bool extract_shebang(const char *line, shellscribe_docblock_t *docblock) {
    if (line == NULL || docblock == NULL) {
        return false;
    }
    if (strncmp(line, "#!", 2) == 0) {
        const char *interpreter = line + 2;
        while (*interpreter == ' ' || *interpreter == '\t') {
            interpreter++;
        }
        
        if (*interpreter) {
            free(docblock->interpreter);
            docblock->interpreter = string_duplicate(interpreter);
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Parse a shell script file and extract documentation blocks
 * 
 * Processes the given shell script file, extracting documentation blocks for the file
 * itself and all functions within it. The function handles shebang lines, file-level
 * metadata, and function documentation.
 * 
 * @param file_path Path to the shell script file to parse
 * @param config Configuration settings controlling parsing behavior
 * @param docblocks Pre-allocated array to store the extracted documentation blocks
 * @param max_blocks Maximum number of blocks that can be stored in the docblocks array
 * 
 * @return int The number of documentation blocks found and processed,
 *             or 0 if an error occurred
 * 
 * @note The first block (index 0) always represents the file-level documentation
 * @see init_docblock
 * @see cleanup_parser_state
 */
int parse_shell_file(const char *file_path, const shellscribe_config_t *config,
    shellscribe_docblock_t *docblocks, int max_blocks) {
    if (file_path == NULL || config == NULL || docblocks == NULL || max_blocks <= 0) {
        return 0;
    }
    for (int i = 0; i < max_blocks; i++) {
        init_docblock(&docblocks[i]);
    }
    docblocks[0].file_name = string_duplicate(file_path);
    parser_state_t state;
    if (!init_parser_state(&state, file_path, config)) {
        debug_message(config, "Failed to initialize parser state\n");
        return 0;
    }
    int block_count = 1;
    state.current_block = &docblocks[0];
    char metadata_line[MAX_LINE_LENGTH];
    rewind(state.file);
    while (fgets(metadata_line, sizeof(metadata_line), state.file)) {
        if (strncmp(metadata_line, "#!", 2) == 0) {
            extract_shebang(metadata_line, &docblocks[0]);
            continue;
        }
        size_t line_len = strlen(metadata_line);
        if (line_len > 0 && metadata_line[line_len - 1] == '\n') {
            metadata_line[line_len - 1] = '\0';
        }
        if (strncmp(metadata_line, "# @", 3) == 0) {
            char *tag = extract_tag_name(metadata_line);
            char *content = extract_tag_content(metadata_line);
            if (tag && content) {
                debug_message(config, "Found metadata tag: %s = %s\n", tag, content);
                if (is_file_level_tag(tag)) {
                    process_file_metadata_tag(&docblocks[0], tag, content);
                }
                
                free(tag);
                free(content);
            }
        } else if (!is_comment_line(metadata_line)) {
            break;
        }
    }
    rewind(state.file);
    state.line_number = 0;
    while (fgets(state.line, sizeof(state.line), state.file) && block_count < max_blocks) {
        state.line_number++;
        size_t len = strlen(state.line);
        if (len > 0 && state.line[len - 1] == '\n') {
            state.line[len - 1] = '\0';
        }
        debug_message(config, "Line %d: %s\n", state.line_number, state.line);
        if (is_comment_line(state.line)) {
            if (is_shellcheck_directive(state.line)) {
                if (state.current_block != NULL && state.current_block != &docblocks[0]) {
                    process_shellcheck_line(state.current_block, state.line);
                }
            }
            if (is_tag_line(state.line)) {
                char *tag = extract_tag_name(state.line);
                char *content = extract_tag_content(state.line);
                if (tag && content) {
                    if (strcmp(tag, "function") == 0) {
                        state.current_block = &docblocks[block_count++];
                        init_docblock(state.current_block);
                    }
                    state_process_tag(&state, tag, content);
                    free(tag);
                    free(content);
                }
            } else if (is_function_declaration(state.line)) {
                debug_message(config, "Found function declaration: %s\n", state.line);
                char *func_name = extract_function_name(state.line);
                if (func_name != NULL) {
                    if (!state.in_docblock || state.current_block == NULL || state.current_block == &docblocks[0]) {
                        state.current_block = &docblocks[block_count++];
                        init_docblock(state.current_block);
                    }
                    if (state.current_block->function_name == NULL) {
                        state.current_block->function_name = func_name;
                    } else {
                        if (strcmp(state.current_block->function_name, func_name) != 0) {
                            debug_message(config, "Warning: Function declaration name mismatch. Expected %s, found %s.\n", state.current_block->function_name, func_name);
                        }
                        free(func_name);
                    }
                    debug_message(config, "Function declaration for %s at line %d\n", state.current_block->function_name, state.line_number);
                    state.in_docblock = false;
                }
            }
        } else {
            state.in_docblock = false;
        }
    }
    cleanup_parser_state(&state);
    
    return block_count;
}

extern void free_docblock(shellscribe_docblock_t *docblock);
extern void free_docblocks(shellscribe_docblock_t *docblocks, int count);

/**
 * @brief Check if a line is a comment line
 * 
 * This function checks if a line is a comment line.
 * 
 * @param line The line to check
 * 
 * @return bool true if the line is a comment, false otherwise
 */
static char *parser_collect_continued_comment_content(parser_state_t *state, const char *initial_content) {
    long pos = ftell(state->file);
    size_t line_size = sizeof(state->line);
    char *accumulated_content = string_duplicate(initial_content);
    debug_message(state->config, "Collecting continued content starting with: '%s'\n", initial_content);
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), state->file) != NULL) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        if (is_comment_line(line) && !is_tag_line(line) && !is_special_annotation(line)) {
            const char *comment_start = strchr(line, '#');
            if (comment_start == NULL) {
                continue;
            }
            comment_start++;
            while (*comment_start && isspace(*comment_start)) {
                comment_start++;
            }
            char *new_content = string_concat(accumulated_content, "\n");
            void *ptr = accumulated_content;
            shell_free(&ptr);
            char *temp = new_content;
            new_content = string_concat(temp, comment_start);
            ptr = temp;
            shell_free(&ptr);
            
            accumulated_content = new_content;
        } else {
            debug_message(state->config, "End of continuation detected: '%s'\n", line);
            fseek(state->file, pos, SEEK_SET);
            break;
        }
        pos = ftell(state->file);
    }
    
    debug_message(state->config, "Collected content: '%s'\n", accumulated_content);
    return accumulated_content;
}

/**
 * @brief Collect continued comment content
 * 
 * This function collects all lines of a continued comment and returns the concatenated content.
 * 
 * @param state The parser state
 * @param initial_content The initial content to start with
 * 
 * @return char* The collected content
 */
char *collect_continued_comment_content(parser_state_t *state, const char *initial_content) {
    return parser_collect_continued_comment_content(state, initial_content);
}

/**
 * @brief Wrapper functions to match the tag_handler_t signature
 */

/**
 * @brief Wrapper for process_file_metadata_tag to match the tag_handler_t signature
 * 
 * @param docblock The documentation block
 * @param content The content of the tag
 * @return bool Result of the process_file_metadata_tag function
 */
static bool process_file_tag_wrapper(shellscribe_docblock_t *docblock, const char *content) {
    return process_file_metadata_tag(docblock, "file", content);
}

/**
 * @brief Wrapper for process_file_metadata_tag (name tag) to match the tag_handler_t signature
 * 
 * @param docblock The documentation block
 * @param content The content of the tag
 * @return bool Result of the process_file_metadata_tag function
 */
static bool process_name_tag_wrapper(shellscribe_docblock_t *docblock, const char *content) {
    return process_file_metadata_tag(docblock, "name", content);
}

/**
 * @brief Wrapper for process_example_tag to match the tag_handler_t signature
 * 
 * @param docblock The documentation block
 * @param content The content of the tag
 * @return bool Result of the add_example_to_docblock function
 */
static bool process_example_tag_wrapper(shellscribe_docblock_t *docblock, const char *content) {
    return false;
}

/**
 * @brief Process a tag internally
 * 
 * This function delegates the processing of a tag to the appropriate handler
 * based on the tag type. It also handles file-level tags and function-level tags.
 * 
 * @param state The parser state
 * @param tag The tag to process
 * @param content The content associated with the tag
 * 
 * @return bool true if the tag was processed successfully, false otherwise
 */
static bool process_tag_internal(parser_state_t *state, const char *tag, const char *content) {
    if (state == NULL || tag == NULL || content == NULL || state->current_block == NULL) {
        return false;
    }
    typedef bool (*tag_handler_t)(shellscribe_docblock_t *, const char *);
    typedef struct {
        const char *tag;
        tag_handler_t handler;
    } tag_handler_entry_t;
    static const tag_handler_entry_t tag_handlers[] = {
        {"brief", process_description_tag},
        {"file", process_file_tag_wrapper},
        {"name", process_name_tag_wrapper},
        {"arg", process_argument_tag},
        {"noargs", NULL},
        {"stdin", process_stdin_tag},
        {"stdout", process_stdout_tag},
        {"stderr", process_stderr_tag},
        {"deprecated", process_deprecated_tag},
        {"replacement", process_replacement_tag},
        {"eol", process_eol_tag},
        {"example", process_example_tag_wrapper},
        {"dependency", process_dependency_tag},
        {"requires", process_requires_tag},
        {"used-by", process_used_by_tag},
        {"calls", process_calls_tag},
        {"provides", process_provides_tag},
        {NULL, NULL}
    };
    if (strcmp(tag, "example") == 0) {
        char *example_content = process_example_tag(state, content);
        if (example_content != NULL) {
            bool result = add_example_to_docblock(state->current_block, example_content);
            free(example_content);
            return result;
        }
        return false;
    }
    for (const tag_handler_entry_t *entry = tag_handlers; entry->tag != NULL; ++entry) {
        if (strcmp(tag, entry->tag) == 0) {
            if (entry->handler) {
                return entry->handler(state->current_block, content);
            }
            return true;
        }
    }

    debug_message(state->config, "Unknown tag: @%s\n", tag);
    return false;
}
