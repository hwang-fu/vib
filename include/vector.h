#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "common.h"
#include "assertion.h"
#include "memory.h"
#include "result.h"

#ifndef VECTOR_DEFAULT_CAPACITY
#define VECTOR_DEFAULT_CAPACITY (20)
#endif // VECTOR_DEFAULT_CAPACITY

#define vector_pushfront(vec, value, cleanup) _vector_pushfront(vec, CAST(value, uintptr_t), cleanup)
#define vector_pushback(vec, value, cleanup) _vector_pushback(vec, CAST(value, uintptr_t), cleanup)

#define vector_try_pushfront(vec, value, cleanup) _vector_try_pushfront(vec, CAST(value, uintptr_t), cleanup)
#define vector_try_pushback(vec, value, cleanup) _vector_try_pushback(vec, CAST(value, uintptr_t), cleanup)

typedef struct vector_t vector_t;
typedef struct vector_item_t vector_item_t;

typedef vector_t * vector_p;
typedef vector_item_t * vector_item_p;

typedef enum vector_init_t
{
    VECTOR_INIT_DEFAULT = 0,
    VECTOR_INIT_CAPACITY_ONLY,
} vector_init_t;

struct vector_t
{
    COPIED uint64_t        capacity;
    COPIED uint64_t        size;
    OWNED  vector_item_p * items;
};

OWNED vector_t * vector_init(OWNED vector_t * vec, uint64_t capacity);

/**
 * Possible overloads:
 * @li OWNED vector_t * mk_vector(VECTOR_INIT_DEFAULT)
 * @li OWNED vector_t * mk_vector(VECTOR_INIT_CAPACITY_ONLY, COPIED uint64_t capacity)
 */
OWNED vector_t * mk_vector(COPIED vector_init_t mode, ...);

COPIED OWNED BORROWED uintptr_t vector_at(BORROWED vector_t * vec, COPIED uint64_t idx);

COPIED OWNED BORROWED uintptr_t vector_front(BORROWED vector_t * vec);

COPIED OWNED BORROWED uintptr_t vector_back(BORROWED vector_t * vec);

COPIED OWNED BORROWED uintptr_t vector_popfront(BORROWED vector_t * vec);

COPIED OWNED BORROWED uintptr_t vector_popback(BORROWED vector_t * vec);

COPIED result_t vector_try_at(BORROWED vector_t * vec, COPIED uint64_t idx);

COPIED result_t vector_try_front(BORROWED vector_t * vec);

COPIED result_t vector_try_back(BORROWED vector_t * vec);

COPIED result_t vector_try_popfront(BORROWED vector_t * vec);

COPIED result_t vector_try_popback(BORROWED vector_t * vec);

void _vector_pushfront(BORROWED vector_t * vec, COPIED OWNED BORROWED uintptr_t value, BORROWED dispose_fn * cleanup);

void _vector_pushback(BORROWED vector_t * vec, COPIED OWNED BORROWED uintptr_t value, BORROWED dispose_fn * cleanup);

COPIED result_t _vector_try_pushfront(BORROWED vector_t * vec, COPIED OWNED BORROWED uintptr_t value, BORROWED dispose_fn * cleanup);

COPIED result_t _vector_try_pushback(BORROWED vector_t * vec, COPIED OWNED BORROWED uintptr_t value, BORROWED dispose_fn * cleanup);

void vector_fit(BORROWED vector_t * vec, COPIED uint64_t new_cap);

COPIED result_t vector_try_fit(BORROWED vector_t * vec, COPIED uint64_t new_cap);

COPIED uint64_t vector_get_size(BORROWED vector_t * vec);

COPIED result_t vector_try_get_size(BORROWED vector_t * vec);

COPIED uint64_t vector_get_capacity(BORROWED vector_t * vec);

COPIED result_t vector_try_get_capacity(BORROWED vector_t * vec);

COPIED bool vector_is_empty(BORROWED vector_t * vec);

COPIED void * vector_dispose(OWNED void * arg);
