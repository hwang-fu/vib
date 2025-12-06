#include "queue.h"

OWNED queue_t * q_init(OWNED queue_t * q, COPIED uint64_t capacity, BORROWED dispose_fn * cleanup)
{
    if (!q)
    {
        q = new(sizeof(queue_t));
    }

    if (EQ(capacity, 0))
    {
        capacity = QUEUE_DEFAULT_CAPACITY;
    }

    q->capacity = capacity;
    q->size     = 0;
    q->elements = new(capacity * sizeof(uintptr_t));
    q->dispose  = cleanup;

    return q;
}

/**
 * Possible overloads:
 * @li OWNED queue_t * mk_q(QUEUE_INIT_DEFAULT)
 * @li OWNED queue_t * mk_q(QUEUE_INIT_CAPACITY_ONLY, uint64_t capacity)
 * @li OWNED queue_t * mk_q(QUEUE_INIT_DISPOSE_ONLY, dispose_fn * cleanup)
 * @li OWNED queue_t * mk_q(QUEUE_INIT_CAPACITY_DISPOSE, uint64_t capacity, dispose_fn * cleanup)
 * @li OWNED queue_t * mk_q(QUEUE_INIT_DISPOSE_CAPACITY, dispose_fn * cleanup, uint64_t capacity)
 */
OWNED queue_t * mk_q(COPIED queue_init_t mode, ...)
{
    va_list ap;
    va_start(ap, mode);

    uint64_t     capacity = QUEUE_DEFAULT_CAPACITY;
    dispose_fn * cleanup  = NIL;
    switch (mode)
    {
        case QUEUE_INIT_DEFAULT:
        {
        } break;

        case QUEUE_INIT_CAPACITY_ONLY:
        {
            capacity = va_arg(ap, uint64_t);
        } break;

        case QUEUE_INIT_DISPOSE_ONLY:
        {
            cleanup = va_arg(ap, dispose_fn*);
        } break;

        case QUEUE_INIT_CAPACITY_DISPOSE:
        {
            capacity = va_arg(ap, uint64_t);
            cleanup  = va_arg(ap, dispose_fn*);
        } break;

        case QUEUE_INIT_DISPOSE_CAPACITY:
        {
            cleanup  = va_arg(ap, dispose_fn*);
            capacity = va_arg(ap, uint64_t);
        } break;

        default:
        {
            PANIC("%s(): Unknown mode %d", __func__, mode);
        } break;
    }

    va_end(ap);
    return q_init(NIL, capacity, cleanup);
}

COPIED OWNED BORROWED uintptr_t q_at(BORROWED queue_t * q, COPIED uint64_t idx)
{
    COPIED result_t result = q_try_at(q, idx);
    if (RESULT_IS_OK(result))
    {
        return RESULT_UNWRAP(result);
    }

    uint64_t errcode = (uint64_t) result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): NIL q argument.", __func__);
        } break;

        case 1:
        {
            PANIC("%s(): The queue size is %lu but trying to access element at index %lu..", __func__, q->size, idx);
        } break;

        default:
        {
            PANIC("%s(): Unknown error: %lu.", __func__, errcode);
        } break;
    }
    return 0; // unreachable, satisfies compiler
}

void _q_unshift(BORROWED queue_t * q, COPIED OWNED BORROWED uintptr_t data)
{
    COPIED result_t result = _q_try_unshift(q, data);
    if (RESULT_IS_OK(result))
    {
        return;
    }

    uint64_t errcode = (uint64_t) result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): NIL q argument.", __func__);
        } break;

        case 1:
        {
            PANIC("%s(): failed to grow the capacity.", __func__);
        } break;

        default:
        {
            PANIC("%s(): Unknown error: %lu.", __func__, errcode);
        } break;
    }
}

void _q_enqueue(BORROWED queue_t * q, COPIED OWNED BORROWED uintptr_t data)
{
    COPIED result_t result = _q_try_enqueue(q, data);
    if (RESULT_IS_OK(result))
    {
        return;
    }

    uint64_t errcode = (uint64_t) result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): NIL q argument.", __func__);
        } break;

        case 1:
        {
            PANIC("%s(): failed to grow the capacity.", __func__);
        } break;

        default:
        {
            PANIC("%s(): Unknown error: %lu.", __func__, errcode);
        } break;
    }
}

COPIED OWNED BORROWED uintptr_t q_peek(BORROWED queue_t * q)
{
    COPIED result_t result = q_try_peek(q);
    if (RESULT_IS_OK(result))
    {
        return RESULT_UNWRAP(result);
    }

    uint64_t errcode = (uint64_t) result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): NIL q argument.", __func__);
        } break;

        case 1:
        {
            PANIC("%s(): Queue is still empty.", __func__);
        } break;

        default:
        {
            PANIC("%s(): Unknown error: %lu.", __func__, errcode);
        } break;
    }
    return 0; // unreachable, satisfies compiler
}

COPIED OWNED BORROWED uintptr_t q_rear(BORROWED queue_t * q)
{
    COPIED result_t result = q_try_rear(q);
    if (RESULT_IS_OK(result))
    {
        return RESULT_UNWRAP(result);
    }

    uint64_t errcode = (uint64_t) result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): NIL q argument.", __func__);
        } break;

        case 1:
        {
            PANIC("%s(): Queue is still empty.", __func__);
        } break;

        default:
        {
            PANIC("%s(): Unknown error: %lu.", __func__, errcode);
        } break;
    }
    return 0; // unreachable, satisfies compiler
}

COPIED OWNED BORROWED uintptr_t q_dequeue(BORROWED queue_t * q)
{
    COPIED result_t result = q_try_dequeue(q);
    if (RESULT_IS_OK(result))
    {
        return RESULT_UNWRAP(result);
    }

    uint64_t errcode = (uint64_t) result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): NIL q argument.", __func__);
        } break;

        case 1:
        {
            PANIC("%s(): Queue is still empty.", __func__);
        } break;

        default:
        {
            PANIC("%s(): Unknown error: %lu.", __func__, errcode);
        } break;
    }
    return 0; // unreachable, satisfies compiler
}

COPIED OWNED BORROWED uintptr_t q_pop(BORROWED queue_t * q)
{
    COPIED result_t result = q_try_pop(q);
    if (RESULT_IS_OK(result))
    {
        return RESULT_UNWRAP(result);
    }

    uint64_t errcode = (uint64_t) result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): NIL q argument.", __func__);
        } break;

        case 1:
        {
            PANIC("%s(): Queue is still empty.", __func__);
        } break;

        default:
        {
            PANIC("%s(): Unknown error.", __func__);
        } break;
    }
    return 0; // unreachable, satisfies compiler
}

COPIED result_t q_try_at(BORROWED queue_t * q, COPIED uint64_t idx)
{
    if (!q)
    {
        return RESULT_ERR(0);
    }

    if (q->size <= idx)
    {
        return RESULT_ERR(1);
    }

    return RESULT_OK(q->elements[idx]);
}

COPIED result_t _q_try_unshift(BORROWED queue_t * q, COPIED OWNED BORROWED uintptr_t data)
{
    if (!q)
    {
        return RESULT_ERR(0);
    }

    uint64_t capacity = q->capacity;
    uint64_t size     = q->size;
    if (WATERMARK(size, capacity) >= WATERMARK_HIGH)
    {
        capacity += 1;
        do
        {
            capacity *= 2;
        } while (WATERMARK(size, capacity) >= WATERMARK_LOW);

        if (!RESULT_IS_OK(q_try_fit(q, capacity)))
        {
            return RESULT_ERR(1);
        }
    }

    memmove(q->elements + 1, q->elements + 0, q->size++ * sizeof(uintptr_t));
    q->elements[0] = data;

    return RESULT_OK(0);
}

COPIED result_t _q_try_enqueue(BORROWED queue_t * q, COPIED OWNED BORROWED uintptr_t data)
{
    if (!q)
    {
        return RESULT_ERR(0);
    }

    uint64_t capacity = q->capacity;
    uint64_t size     = q->size;
    if (WATERMARK(size, capacity) >= WATERMARK_HIGH)
    {
        capacity += 1;
        do
        {
            capacity *= 2;
        } while (WATERMARK(size, capacity) >= WATERMARK_LOW);

        if (!RESULT_IS_OK(q_try_fit(q, capacity)))
        {
            return RESULT_ERR(1);
        }
    }

    q->elements[q->size++] = data;

    return RESULT_OK(0);
}

COPIED result_t q_try_peek(BORROWED queue_t * q)
{
    if (!q)
    {
        return RESULT_ERR(0);
    }

    if (EQ(q->size, 0))
    {
        return RESULT_ERR(1);
    }

    return RESULT_OK(q->elements[0]);
}

COPIED result_t q_try_rear(BORROWED queue_t * q)
{
    if (!q)
    {
        return RESULT_ERR(0);
    }

    uint64_t size = q->size;
    if (EQ(size, 0))
    {
        return RESULT_ERR(1);
    }

    return RESULT_OK(q->elements[size-1]);
}

COPIED result_t q_try_dequeue(BORROWED queue_t * q)
{
    if (!q)
    {
        return RESULT_ERR(0);
    }

    if (EQ(q->size, 0))
    {
        return RESULT_ERR(1);
    }

    uintptr_t data = q->elements[0];

    memmove(q->elements + 0, q->elements + 1, (--q->size) * sizeof(uintptr_t));

    return RESULT_OK(data);
}

COPIED result_t q_try_pop(BORROWED queue_t * q)
{
    if (!q)
    {
        return RESULT_ERR(0);
    }

    if (EQ(q->size, 0))
    {
        return RESULT_ERR(1);
    }

    return RESULT_OK(q->elements[--q->size]);
}

COPIED uint64_t q_get_size(BORROWED queue_t * q)
{
    SCP(q);
    return q->size;
}

COPIED result_t q_try_get_size(BORROWED queue_t * q)
{
    if (!q)
    {
        return RESULT_ERR(0);
    }

    return RESULT_OK(q->size);
}

COPIED uint64_t q_get_capacity(BORROWED queue_t * q)
{
    SCP(q);
    return q->capacity;
}

COPIED result_t q_try_get_capacity(BORROWED queue_t * q)
{
    if (!q)
    {
        return RESULT_ERR(0);
    }

    return RESULT_OK(q->capacity);
}

void q_fit(BORROWED queue_t * q, COPIED uint64_t new_cap)
{
    COPIED result_t result = q_try_fit(q, new_cap);
    if (RESULT_IS_OK(result))
    {
        return;
    }

    uint64_t errcode = (uint64_t) result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): NIL q argument.", __func__);
        } break;

        case 1:
        {
            PANIC(
                "%s(): the old capacity is %lu greater than or equal to the new capacity is %lu.",
                __func__, q->capacity, new_cap);
        } break;

        default:
        {
            PANIC("%s(): Unknown error: %d.", __func__, errcode);
        } break;
    }
}

COPIED result_t q_try_fit(BORROWED queue_t * q, COPIED uint64_t new_cap)
{
    if (!q)
    {
        return RESULT_ERR(0);
    }

    uint64_t old_cap = q->capacity;
    if (old_cap >= new_cap)
    {
        return RESULT_ERR(1);
    }

    q->elements = realloc_smart(q->elements, new_cap * sizeof(uintptr_t));
    q->capacity = new_cap;

    return RESULT_OK(0);
}

COPIED bool q_is_empty(BORROWED queue_t * q)
{
    return (!q)
        ? true
        : EQ(q->size, 0);
}

COPIED void * q_dispose(OWNED void * arg)
{
    if (!arg)
    {
        return NIL;
    }

    OWNED queue_t * q = CAST(arg, queue_t*);

    uint64_t size = q->size;
    if (q->dispose)
    {
        dispose_fn * cleanup = q->dispose;
        for (uint64_t i = 0; i < size; i++)
        {
            cleanup(CAST(q->elements[i], void*));
        }
    }
    free_smart(q->elements);

    return dispose(q);
}
