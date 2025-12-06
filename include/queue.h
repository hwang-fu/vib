#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "common.h"
#include "result.h"
#include "memory.h"
#include "assertion.h"

#ifndef QUEUE_DEFAULT_CAPACITY
#define QUEUE_DEFAULT_CAPACITY (20)
#endif // QUEUE_DEFAULT_CAPACITY

#define q_unshift(q, data) _q_unshift(q, CAST((data), uintptr_t))
#define q_enqueue(q, data)  _q_enqueue(q, CAST((data), uintptr_t))

#define q_try_unshift(q, data) _q_try_unshift(q, CAST((data), uintptr_t))
#define q_try_enqueue(q, data)  _q_try_enqueue(q, CAST((data), uintptr_t))

typedef struct queue_t queue_t;

typedef enum queue_init_t
{
    QUEUE_INIT_DEFAULT = 0,
    QUEUE_INIT_CAPACITY_ONLY,
    QUEUE_INIT_DISPOSE_ONLY,
    QUEUE_INIT_CAPACITY_DISPOSE,
    QUEUE_INIT_DISPOSE_CAPACITY,
} queue_init_t;

struct queue_t
{
    COPIED   uint64_t     size      ;
    COPIED   uint64_t     capacity  ;
    OWNED    uintptr_t  * elements  ;
    BORROWED dispose_fn * dispose   ;
};

OWNED queue_t * q_init(OWNED queue_t * q, COPIED uint64_t capacity, BORROWED dispose_fn * cleanup);

/**
 * Possible overloads:
 * @li OWNED queue_t * mk_q(QUEUE_INIT_DEFAULT)
 * @li OWNED queue_t * mk_q(QUEUE_INIT_CAPACITY_ONLY, uint64_t capacity)
 * @li OWNED queue_t * mk_q(QUEUE_INIT_DISPOSE_ONLY, dispose_fn * cleanup)
 * @li OWNED queue_t * mk_q(QUEUE_INIT_CAPACITY_DISPOSE, uint64_t capacity, dispose_fn * cleanup)
 * @li OWNED queue_t * mk_q(QUEUE_INIT_DISPOSE_CAPACITY, dispose_fn * cleanup, uint64_t capacity)
 */
OWNED queue_t * mk_q(COPIED queue_init_t mode, ...);

COPIED OWNED BORROWED uintptr_t q_at(BORROWED queue_t * q, COPIED uint64_t idx);

void _q_unshift(BORROWED queue_t * q, COPIED OWNED BORROWED uintptr_t data);

void _q_enqueue(BORROWED queue_t * q, COPIED OWNED BORROWED uintptr_t data);

COPIED OWNED BORROWED uintptr_t q_peek(BORROWED queue_t * q);

COPIED OWNED BORROWED uintptr_t q_rear(BORROWED queue_t * q);

COPIED OWNED BORROWED uintptr_t q_dequeue(BORROWED queue_t * q);

COPIED OWNED BORROWED uintptr_t q_pop(BORROWED queue_t * q);

COPIED result_t q_try_at(BORROWED queue_t * q, COPIED uint64_t idx);

COPIED result_t _q_try_unshift(BORROWED queue_t * q, COPIED OWNED BORROWED uintptr_t data);

COPIED result_t _q_try_enqueue(BORROWED queue_t * q, COPIED OWNED BORROWED uintptr_t data);

COPIED result_t q_try_peek(BORROWED queue_t * q);

COPIED result_t q_try_rear(BORROWED queue_t * q);

COPIED result_t q_try_dequeue(BORROWED queue_t * q);

COPIED result_t q_try_pop(BORROWED queue_t * q);

COPIED uint64_t q_get_size(BORROWED queue_t * q);

COPIED result_t q_try_get_size(BORROWED queue_t * q);

COPIED uint64_t q_get_capacity(BORROWED queue_t * q);

COPIED result_t q_try_get_capacity(BORROWED queue_t * q);

void q_fit(BORROWED queue_t * q, COPIED uint64_t new_cap);

COPIED result_t q_try_fit(BORROWED queue_t * q, COPIED uint64_t new_cap);

/**
 * @brief       If @param {q} is @const {NIL}, the function also returns @const {true}.
 */
COPIED bool q_is_empty(BORROWED queue_t * q);

COPIED void * q_dispose(OWNED void * arg);
