#pragma once

#include <stdlib.h>

#include "common.h"
#include "assertion.h"

// -------------------------------------------------------------
// | Memory Safety Allocator Macros|
// -------------------------------------------------------------
/**
 * free_smart(ptr):
 *      1. Frees the pointer if not NIL.
 *      2. Sets the pointer to NIL after freeing.
 */
#define free_smart(ptr)                                             \
        do {                                                        \
            if ( (ptr) ) free(CAST(ptr, void*));                    \
            ptr = NIL;                                              \
        } while (0)

/**
 * new(bytes):
 *      1. Allocates `n` bytes from the heap.
 *      2. Aborts the program if failed to allocate.
 */
#define new(n)                                                      \
        new_( n )

/**
 * zeros(bytes):
 *      1. Allocates `n` bytes from the heap and nullify them all.
 *      2. Aborts the program if failed to allocate.
 */
#define zeros(n)                                                    \
        zeros_( n )


// -------------------------------------------------------------
// | Memory Allocation Helpers |
// -------------------------------------------------------------
OWNED void * new_(COPIED uint64_t bytes);
OWNED void * zeros_(COPIED uint64_t bytes);
OWNED void * realloc_smart(OWNED void * arg, COPIED uint64_t new_bytes);
COPIED void * dispose(OWNED void * arg);
