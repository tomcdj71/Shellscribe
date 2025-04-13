/**
 * @file memory.h
 * @brief Memory management utilities for shellscribe
 * 
 * This module provides memory management utilities designed to:
 * - Prevent memory leaks
 * - Handle allocation failures gracefully
 * - Provide consistent error reporting
 * - Add optional debugging capabilities for memory usage
 */

#ifndef SHELLSCRIBE_MEMORY_H
#define SHELLSCRIBE_MEMORY_H

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Allocate memory with error checking
 * 
 * Wrapper around malloc that includes error checking.
 * 
 * @param size Size of memory to allocate in bytes
 * @return void* Pointer to allocated memory, NULL on failure
 */
void *shell_malloc(size_t size);

/**
 * @brief Allocate and zero-initialize memory with error checking
 * 
 * Wrapper around calloc that includes error checking.
 * 
 * @param count Number of elements
 * @param size Size of each element in bytes
 * @return void* Pointer to allocated memory, NULL on failure
 */
void *shell_calloc(size_t count, size_t size);

/**
 * @brief Reallocate memory with error checking
 * 
 * Wrapper around realloc that includes error checking.
 * 
 * @param ptr Pointer to memory to reallocate
 * @param size New size in bytes
 * @return void* Pointer to reallocated memory, NULL on failure
 */
void *shell_realloc(void *ptr, size_t size);

/**
 * @brief Free memory and set pointer to NULL
 * 
 * Wrapper around free that also sets the pointer to NULL to prevent use-after-free.
 * 
 * @param ptr_ref Pointer to the pointer that should be freed
 */
void shell_free(void **ptr_ref);

/**
 * @brief Register an external allocation with the memory tracker
 * 
 * This is used to track memory allocated by external functions like strdup().
 * 
 * @param ptr Pointer to the allocated memory
 * @param size Size of the allocation in bytes
 */
void shell_register_external(void *ptr, size_t size);

/**
 * @brief Duplicate a string with error checking
 * 
 * Allocates memory and copies a string into it.
 * 
 * @param str String to duplicate
 * @return char* Pointer to the new string, NULL on failure
 */
char *shell_strdup(const char *str);

/**
 * @brief Print memory usage statistics
 * 
 * When memory tracking is enabled, prints statistics about memory usage.
 */
void shell_memory_stats(void);

/**
 * @brief Check for memory leaks
 * 
 * When memory tracking is enabled, checks if there are any memory leaks.
 * 
 * @return bool true if there are leaks, false otherwise
 */
bool shell_check_leaks(void);

/**
 * @brief Enable memory tracking
 * 
 * Enables detailed memory tracking for debugging purposes.
 * This will slow down execution but provides detailed information about allocations.
 */
void shell_memory_tracking_enable(void);

/**
 * @brief Disable memory tracking
 * 
 * Disables detailed memory tracking.
 */
void shell_memory_tracking_disable(void);

/**
 * @brief Clean up all tracked memory
 * 
 * Frees all memory allocations that are still being tracked.
 * This should only be called at program exit to ensure no memory leaks.
 */
void shell_memory_cleanup(void);

/**
 * @brief Check if memory tracking is enabled
 * 
 * @return bool true if memory tracking is enabled, false otherwise
 */
bool shell_memory_tracking_is_enabled(void);

#endif /* SHELLSCRIBE_MEMORY_H */ 
