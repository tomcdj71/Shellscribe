/**
 * @file memory.c
 * @brief Implementation of memory management utilities and tracking
 * 
 * This file provides memory management functions with built-in error checking,
 * memory leak detection, and allocation tracking capabilities. It helps ensure
 * robust memory management throughout the application and provides diagnostic
 * tools to identify memory-related issues.
 */

#include "utils/memory.h"
#include "utils/debug.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/**
 * @brief Number of stack frames to capture for each allocation
 */
#define BACKTRACE_DEPTH 4

/**
 * @brief Maximum number of tracked allocations
 */
#define MAX_TRACKED_ALLOCATIONS 1000

/**
 * @brief Maximum number of memory cleanups to track
 */
#define MAX_CLEANUP_POINTERS 2000

/**
 * @brief Structure to hold information about a memory allocation
 */
typedef struct {
    void *ptr;           // Pointer to the allocated memory
    size_t size;         // Size of the allocation
    const char *file;    // Source file where allocation occurred
    int line;            // Line number where allocation occurred
    void *backtrace[BACKTRACE_DEPTH];  // Call stack at allocation time
    int backtrace_size;  // Actual size of backtrace
} allocation_info_t;

/**
 * @brief Flag indicating if memory tracking is enabled
 */
static bool memory_tracking_enabled = false;

/**
 * @brief Count of currently allocated memory blocks
 */
static size_t allocation_count = 0;

/**
 * @brief Total size of currently allocated memory
 */
static size_t total_allocated_size = 0;

/**
 * @brief Maximum memory usage reached
 */
static size_t peak_memory_usage = 0;

/**
 * @brief Array of tracked allocations
 */
static allocation_info_t tracked_allocations[MAX_TRACKED_ALLOCATIONS];

/**
 * @brief Array to track pointers that have been freed during cleanup
 */
static void *freed_pointers[MAX_CLEANUP_POINTERS];
static int freed_count = 0;

/**
 * @brief Register a new allocation in the tracking system
 * 
 * Records a newly allocated pointer in the memory tracking system, updating
 * the allocation count and memory usage statistics.
 * 
 * @param ptr Pointer to the allocated memory
 * @param size Size of the allocated memory in bytes
 * 
 * @note This function has no effect if memory tracking is disabled or if ptr is NULL
 */
static void register_allocation(void *ptr, size_t size) {
    if (!memory_tracking_enabled || ptr == NULL) {
        return;
    }
    for (int i = 0; i < MAX_TRACKED_ALLOCATIONS; i++) {
        if (tracked_allocations[i].ptr == NULL) {
            tracked_allocations[i].ptr = ptr;
            tracked_allocations[i].size = size;
            tracked_allocations[i].file = "unknown";
            tracked_allocations[i].line = 0;
            tracked_allocations[i].backtrace_size = 0;
            allocation_count++;
            total_allocated_size += size;
            if (total_allocated_size > peak_memory_usage) {
                peak_memory_usage = total_allocated_size;
            }
            
            return;
        }
    }
    
    fprintf(stderr, "Warning: Maximum number of tracked allocations exceeded\n");
}

/**
 * @brief Register an external allocation with the memory tracker
 * 
 * Allows externally allocated memory (e.g., from third-party libraries)
 * to be tracked by the memory management system.
 * 
 * @param ptr Pointer to the externally allocated memory
 * @param size Size of the allocated memory in bytes
 * 
 * @note This function has no effect if memory tracking is disabled
 */
void shell_register_external(void *ptr, size_t size) {
    register_allocation(ptr, size);
}

/**
 * @brief Unregister an allocation from the tracking system
 * 
 * Removes a pointer from the memory tracking system when it is freed,
 * updating the allocation count and memory usage statistics.
 * 
 * @param ptr Pointer to the memory being freed
 * 
 * @note This function has no effect if memory tracking is disabled or if ptr is NULL
 * @warning Will issue a warning if attempting to free an untracked pointer
 */
static void unregister_allocation(void *ptr) {
    if (!memory_tracking_enabled || ptr == NULL) {
        return;
    }
    for (int i = 0; i < MAX_TRACKED_ALLOCATIONS; i++) {
        if (tracked_allocations[i].ptr == ptr) {
            total_allocated_size -= tracked_allocations[i].size;
            allocation_count--;
            tracked_allocations[i].ptr = NULL;
            tracked_allocations[i].size = 0;
            tracked_allocations[i].file = NULL;
            tracked_allocations[i].line = 0;
            tracked_allocations[i].backtrace_size = 0;
            
            return;
        }
    }
    
    fprintf(stderr, "Warning: attempt to free untracked pointer %p\n", ptr);
}

/**
 * @brief Allocate memory with error checking
 * 
 * Allocates memory with built-in error checking and automatic tracking
 * of the allocation if memory tracking is enabled.
 * 
 * @param size Number of bytes to allocate
 * 
 * @return void* Pointer to the allocated memory, or NULL if allocation fails
 *               or if the requested size is zero
 * 
 * @see shell_free
 */
void *shell_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Error: Failed to allocate %zu bytes of memory\n", size);
        return NULL;
    }

    register_allocation(ptr, size);
    return ptr;
}

/**
 * @brief Allocate and zero-initialize memory with error checking
 * 
 * Allocates memory and initializes it to zero, with built-in error checking
 * and automatic tracking of the allocation if memory tracking is enabled.
 * 
 * @param count Number of elements to allocate
 * @param size Size of each element in bytes
 * 
 * @return void* Pointer to the allocated memory, or NULL if allocation fails
 *               or if either count or size is zero
 * 
 * @see shell_free
 */
void *shell_calloc(size_t count, size_t size) {
    if (count == 0 || size == 0) {
        return NULL;
    }
    void *ptr = calloc(count, size);
    if (ptr == NULL) {
        fprintf(stderr, "Error: Failed to allocate %zu bytes of memory\n", count * size);
        return NULL;
    }

    register_allocation(ptr, count * size);
    return ptr;
}

/**
 * @brief Reallocate memory with error checking
 * 
 * Changes the size of a previously allocated memory block, with built-in error
 * checking and tracking of the allocation if memory tracking is enabled.
 * 
 * @param ptr Pointer to the memory block to resize (can be NULL)
 * @param size New size in bytes (can be 0)
 * 
 * @return void* Pointer to the reallocated memory, or NULL if reallocation fails,
 *               if size is 0, or if ptr is NULL and size is 0
 * 
 * @note If ptr is NULL, this function behaves like shell_malloc
 * @note If size is 0 and ptr is not NULL, the memory is freed and NULL is returned
 * @note If reallocation fails, the original block is left untouched
 * 
 * @see shell_malloc
 * @see shell_free
 */
void *shell_realloc(void *ptr, size_t size) {
    if (size == 0) {
        if (ptr != NULL) {
            unregister_allocation(ptr);
            free(ptr);
        }
        return NULL;
    }

    if (ptr == NULL) {
        return shell_malloc(size);
    }

    if (memory_tracking_enabled) {
        unregister_allocation(ptr);
    }

    void *new_ptr = realloc(ptr, size);
    if (new_ptr == NULL) {
        fprintf(stderr, "Error: Failed to reallocate memory to %zu bytes\n", size);
        if (memory_tracking_enabled && ptr != NULL) {
            register_allocation(ptr, 0);
        }
        return NULL;
    }
    register_allocation(new_ptr, size);
    return new_ptr;
}

/**
 * @brief Free memory and set pointer to NULL
 * 
 * Frees allocated memory and sets the pointer to NULL to prevent use-after-free issues.
 * Also unregisters the allocation from the memory tracking system if tracking is enabled.
 *
 * @param ptr_ref Pointer to a pointer that will be freed and set to NULL
 * 
 * @note If ptr_ref is NULL or *ptr_ref is NULL, the function does nothing
 */
void shell_free(void **ptr_ref) {
    if (ptr_ref == NULL || *ptr_ref == NULL) {
        return;
    }

    unregister_allocation(*ptr_ref);
    free(*ptr_ref);
    *ptr_ref = NULL;
}

/**
 * @brief Duplicate a string with error checking
 * 
 * Creates a new copy of a string with built-in error checking and
 * automatically tracks the allocation if memory tracking is enabled.
 *
 * @param str The string to duplicate
 * 
 * @return char* Pointer to the newly allocated string, or NULL if str is NULL
 *               or if memory allocation fails
 */
char *shell_strdup(const char *str) {
    if (str == NULL) {
        return NULL;
    }

    size_t len = strlen(str) + 1;
    char *dup = (char *)shell_malloc(len);
    if (dup == NULL) {
        return NULL;
    }

    memcpy(dup, str, len);
    return dup;
}

/**
 * @brief Print memory usage statistics
 * 
 * Outputs detailed information about current memory usage, including the number
 * of active allocations, total memory used, and peak memory usage. If there are
 * active allocations, it will list them.
 * 
 * @note This function has no effect if memory tracking is disabled
 */
void shell_memory_stats(void) {
    if (!memory_tracking_enabled) {
        fprintf(stderr, "Memory tracking is not enabled\n");
        return;
    }

    fprintf(stderr, "Memory Statistics:\n");
    fprintf(stderr, "  Current allocations: %zu\n", allocation_count);
    fprintf(stderr, "  Current allocated memory: %zu bytes\n", total_allocated_size);
    fprintf(stderr, "  Peak memory usage: %zu bytes\n", peak_memory_usage);
    if (allocation_count > 0) {
        fprintf(stderr, "\nActive allocations:\n");
        for (int i = 0; i < MAX_TRACKED_ALLOCATIONS; i++) {
            if (tracked_allocations[i].ptr != NULL) {
                fprintf(stderr, "  %p: %zu bytes\n", 
                        tracked_allocations[i].ptr, 
                        tracked_allocations[i].size);
            }
        }
    }
}

/**
 * @brief Check for memory leaks
 * 
 * Determines if there are any unfreed memory allocations that are being tracked.
 * 
 * @return bool true if there are memory leaks detected, false otherwise or if
 *              memory tracking is disabled
 */
bool shell_check_leaks(void) {
    if (!memory_tracking_enabled) {
        fprintf(stderr, "Memory tracking is not enabled\n");
        return false;
    }

    if (allocation_count > 0) {
        fprintf(stderr, "Memory leak detected: %zu allocations still active\n", allocation_count);
        return true;
    }

    return false;
}

/**
 * @brief Enable memory tracking
 * 
 * Initializes and enables the memory tracking system. Once enabled, all memory
 * allocations made through shell_malloc, shell_calloc, etc. will be tracked.
 */
void shell_memory_tracking_enable(void) {
    for (int i = 0; i < MAX_TRACKED_ALLOCATIONS; i++) {
        tracked_allocations[i].ptr = NULL;
        tracked_allocations[i].size = 0;
        tracked_allocations[i].file = NULL;
        tracked_allocations[i].line = 0;
        tracked_allocations[i].backtrace_size = 0;
    }
    
    memory_tracking_enabled = true;
    allocation_count = 0;
    total_allocated_size = 0;
    peak_memory_usage = 0;
}

/**
 * @brief Disable memory tracking
 * 
 * Turns off the memory tracking system. After calling this function, memory
 * allocations will no longer be tracked, but existing tracking data is preserved.
 */
void shell_memory_tracking_disable(void) {
    memory_tracking_enabled = false;
}

/**
 * @brief Check if a pointer has already been freed during cleanup
 * 
 * Verifies if a given pointer has already been marked as freed during the
 * cleanup process to prevent double-free errors.
 *
 * @param ptr The pointer to check
 * 
 * @return bool true if the pointer has already been freed, false otherwise
 */
static bool is_already_freed(void *ptr) {
    for (int i = 0; i < freed_count; i++) {
        if (freed_pointers[i] == ptr) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Record a pointer as freed
 * 
 * Adds a pointer to the list of pointers that have been freed during cleanup
 * to prevent double-free errors.
 *
 * @param ptr The pointer to record as freed
 */
static void record_freed_pointer(void *ptr) {
    if (freed_count < MAX_CLEANUP_POINTERS) {
        freed_pointers[freed_count++] = ptr;
    }
}

/**
 * @brief Clean up all tracked memory
 * 
 * Frees all memory allocations currently being tracked by the memory tracking system.
 * This function is useful for cleaning up all memory at program exit.
 * 
 * @note This function has no effect if memory tracking is disabled
 */
void shell_memory_cleanup(void) {
    if (!memory_tracking_enabled) {
        return;
    }
    fprintf(stderr, "Memory cleanup: freeing %zu allocations\n", allocation_count);
    bool duplicates_found = false;
    for (int i = 0; i < MAX_TRACKED_ALLOCATIONS; i++) {
        if (tracked_allocations[i].ptr == NULL) {
            continue;
        }
        for (int j = i + 1; j < MAX_TRACKED_ALLOCATIONS; j++) {
            if (tracked_allocations[j].ptr == tracked_allocations[i].ptr) {
                fprintf(stderr, "Found duplicate pointer in tracking array: %p\n", 
                        tracked_allocations[i].ptr);
                duplicates_found = true;
                tracked_allocations[j].ptr = NULL;
            }
        }
    }
    if (duplicates_found) {
        fprintf(stderr, "WARNING: Duplicate pointers detected. Memory will leak but program will not crash.\n");
        for (int i = 0; i < MAX_TRACKED_ALLOCATIONS; i++) {
            tracked_allocations[i].ptr = NULL;
            tracked_allocations[i].size = 0;
            tracked_allocations[i].file = NULL;
            tracked_allocations[i].line = 0;
            tracked_allocations[i].backtrace_size = 0;
        }
    } else {
        size_t freed_count = 0;
        for (int i = 0; i < MAX_TRACKED_ALLOCATIONS; i++) {
            if (tracked_allocations[i].ptr != NULL) {
                free(tracked_allocations[i].ptr);
                freed_count++;
                tracked_allocations[i].ptr = NULL;
                tracked_allocations[i].size = 0;
                tracked_allocations[i].file = NULL;
                tracked_allocations[i].line = 0;
                tracked_allocations[i].backtrace_size = 0;
            }
        }
        
        fprintf(stderr, "Successfully freed %zu allocations\n", freed_count);
    }

    allocation_count = 0;
    total_allocated_size = 0;
    freed_count = 0;
    for (int i = 0; i < MAX_CLEANUP_POINTERS; i++) {
        freed_pointers[i] = NULL;
    }
    fprintf(stderr, "Memory tracking reset\n");
}

/**
 * @brief Check if memory tracking is enabled
 * 
 * @return bool true if memory tracking is currently enabled, false otherwise
 */
bool shell_memory_tracking_is_enabled(void) {
    return memory_tracking_enabled;
}
