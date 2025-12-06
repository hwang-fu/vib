#include "vector.h"

struct vector_item_t
{
    COPIED OWNED BORROWED uintptr_t    value;
    BORROWED              dispose_fn * dispose;
};

static OWNED vector_item_t * mk_vector_item_(COPIED OWNED BORROWED uintptr_t value, BORROWED dispose_fn * cleanup);
static COPIED void * vector_item_dispose_(OWNED void * arg);

static OWNED vector_item_t * mk_vector_item_(COPIED OWNED BORROWED uintptr_t value, BORROWED dispose_fn * cleanup)
{
    OWNED vector_item_p item = new(sizeof(vector_item_t));
    item->value              = value;
    item->dispose            = cleanup;
    return item;
}

static COPIED void * vector_item_dispose_(OWNED void * arg)
{
    if (!arg)
    {
        return NIL;
    }

    OWNED vector_item_p item = CAST(arg, vector_item_p);
    if (item->dispose)
    {
        item->dispose(CAST(item->value, void*));
    }
    return dispose(item);
}



OWNED vector_t * vector_init(OWNED vector_t * vec, COPIED uint64_t capacity)
{
    if (!vec)
    {
        vec = new(sizeof(vector_t));
    }

    vec->size     = 0;
    vec->capacity = capacity;
    vec->items    = new(capacity * sizeof(vector_item_t*));

    return vec;
}

/**
 * Possible overloads:
 * @li OWNED vector_t * mk_vector(VECTOR_INIT_DEFAULT)
 * @li OWNED vector_t * mk_vector(VECTOR_INIT_CAPACITY_ONLY, COPIED uint64_t capacity)
 */
OWNED vector_t * mk_vector(COPIED vector_init_t mode, ...)
{
    va_list ap;
    va_start(ap, mode);

    uint64_t     capacity = VECTOR_DEFAULT_CAPACITY;
    switch (mode)
    {
        case VECTOR_INIT_DEFAULT:
        {
        } break;

        case VECTOR_INIT_CAPACITY_ONLY:
        {
            capacity = va_arg(ap, uint64_t);
        } break;

        default:
        {
            PANIC("%s(): unkown mode %d", __func__, mode);
        } break;
    }

    va_end(ap);
    return vector_init(NIL, capacity);
}

COPIED OWNED BORROWED uintptr_t vector_at(BORROWED vector_t * vec, COPIED uint64_t idx)
{
    COPIED result_t result = vector_try_at(vec, idx);
    if (RESULT_IS_OK(result))
    {
        return RESULT_UNWRAP(result);
    }

    uint64_t errcode = (uint64_t) result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): NIL vector argument.", __func__);
        } break;

        case 1:
        {
            PANIC("%s(): the vector size is %lu but trying to access element at index %lu..", __func__, vec->size, idx);
        } break;

        default:
        {
            PANIC("%s(): Unknown error: %lu.", __func__, errcode);
        } break;
    }
    return 0; // unreachable, satisfies compiler
}

COPIED OWNED BORROWED uintptr_t vector_front(BORROWED vector_t * vec)
{
    COPIED result_t result = vector_try_front(vec);
    if (RESULT_IS_OK(result))
    {
        return RESULT_UNWRAP(result);
    }

    uint64_t errcode = (uint64_t) result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): NIL vector argument.", __func__);
        } break;

        case 1:
        {
            PANIC("%s(): the vector is still empty.", __func__);
        } break;

        default:
        {
            PANIC("%s(): Unknown error.", __func__);
        } break;
    }
    return 0; // unreachable, satisfies compiler
}

COPIED OWNED BORROWED uintptr_t vector_back(BORROWED vector_t * vec)
{
    COPIED result_t result = vector_try_back(vec);
    if (RESULT_IS_OK(result))
    {
        return RESULT_UNWRAP(result);
    }

    uint64_t errcode = (uint64_t) result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): NIL vector argument.", __func__);
        } break;

        case 1:
        {
            PANIC("%s(): the vector is still empty.", __func__);
        } break;

        default:
        {
            PANIC("%s(): Unknown error.", __func__);
        } break;
    }
    return 0; // unreachable, satisfies compiler
}

COPIED OWNED BORROWED uintptr_t vector_popfront(BORROWED vector_t * vec)
{
    COPIED result_t result = vector_try_popfront(vec);
    if (RESULT_IS_OK(result))
    {
        return RESULT_UNWRAP(result);
    }

    uint64_t errcode = (uint64_t) result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): NIL vector argument.", __func__);
        } break;

        case 1:
        {
            PANIC("%s(): the vector is still empty.", __func__);
        } break;

        default:
        {
            PANIC("%s(): Unknown error.", __func__);
        } break;
    }
    return 0; // unreachable, satisfies compiler
}

COPIED OWNED BORROWED uintptr_t vector_popback(BORROWED vector_t * vec)
{
    COPIED result_t result = vector_try_popback(vec);
    if (RESULT_IS_OK(result))
    {
        return RESULT_UNWRAP(result);
    }

    uint64_t errcode = (uint64_t) result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): NIL vector argument.", __func__);
        } break;

        case 1:
        {
            PANIC("%s(): the vector is still empty.", __func__);
        } break;

        default:
        {
            PANIC("%s(): Unknown error.", __func__);
        } break;
    }
    return 0; // unreachable, satisfies compiler
}

COPIED result_t vector_try_at(BORROWED vector_t * vec, COPIED uint64_t idx)
{
    if (!vec)
    {
        return RESULT_ERR(0);
    }

    if (vec->size <= idx)
    {
        return RESULT_ERR(1);
    }

    return RESULT_OK(vec->items[idx]->value);
}

COPIED result_t vector_try_front(BORROWED vector_t * vec)
{
    if (!vec)
    {
        return RESULT_ERR(0);
    }

    if (EQ(vec->size, 0))
    {
        return RESULT_ERR(1);
    }

    return RESULT_OK(vec->items[0]->value);
}

COPIED result_t vector_try_back(BORROWED vector_t * vec)
{
    if (!vec)
    {
        return RESULT_ERR(0);
    }

    uint64_t size = vec->size;
    if (EQ(size, 0))
    {
        return RESULT_ERR(1);
    }

    return RESULT_OK(vec->items[size-1]->value);
}

COPIED result_t vector_try_popfront(BORROWED vector_t * vec)
{
    if (!vec)
    {
        return RESULT_ERR(0);
    }

    if (EQ(vec->size, 0))
    {
        return RESULT_ERR(1);
    }

    vector_item_p item = vec->items[0];
    uintptr_t data = item->value;
    dispose(item);

    memmove(vec->items + 0, vec->items + 1, (--vec->size) * sizeof(vector_item_t*));

    return RESULT_OK(data);
}

COPIED result_t vector_try_popback(BORROWED vector_t * vec)
{
    if (!vec)
    {
        return RESULT_ERR(0);
    }

    if (EQ(vec->size, 0))
    {
        return RESULT_ERR(1);
    }

    vector_item_p item = vec->items[--vec->size];
    uintptr_t data = item->value;
    dispose(item);

    return RESULT_OK(data);
}

void _vector_pushfront(BORROWED vector_t * vec, COPIED OWNED BORROWED uintptr_t value, BORROWED dispose_fn * cleanup)
{
    COPIED result_t result = _vector_try_pushfront(vec, value, cleanup);
    if (RESULT_IS_OK(result))
    {
        return;
    }

    uint64_t errcode = (uint64_t) result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): NIL vector argument.", __func__);
        } break;

        case 1:
        {
            PANIC("%s(): failed to grow the capacity.", __func__);
        } break;

        default:
        {
            PANIC("%s(): Unknown error.", __func__);
        } break;
    }
}

void _vector_pushback(BORROWED vector_t * vec, COPIED OWNED BORROWED uintptr_t value, BORROWED dispose_fn * cleanup)
{
    COPIED result_t result = _vector_try_pushback(vec, value, cleanup);
    if (RESULT_IS_OK(result))
    {
        return;
    }

    uint64_t errcode = (uint64_t) result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): NIL vector argument.", __func__);
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

COPIED result_t _vector_try_pushfront(BORROWED vector_t * vec, COPIED OWNED BORROWED uintptr_t value, BORROWED dispose_fn * cleanup)
{
    if (!vec)
    {
        return RESULT_ERR(0);
    }

    uint64_t capacity = vec->capacity;
    uint64_t size     = vec->size;
    if (WATERMARK(size, capacity) >= WATERMARK_HIGH)
    {
        capacity += 1;
        do
        {
            capacity *= 2;
        } while (WATERMARK(size, capacity) >= WATERMARK_LOW);

        if (!RESULT_IS_OK(vector_try_fit(vec, capacity)))
        {
            return RESULT_ERR(1);
        }
    }

    memmove(vec->items + 1, vec->items + 0, vec->size++ * sizeof(vector_item_t*));
    vec->items[0] = mk_vector_item_(value, cleanup);

    return RESULT_OK(0);
}

COPIED result_t _vector_try_pushback(BORROWED vector_t * vec, COPIED OWNED BORROWED uintptr_t value, BORROWED dispose_fn * cleanup)
{
    if (!vec)
    {
        return RESULT_ERR(0);
    }

    uint64_t capacity = vec->capacity;
    uint64_t size     = vec->size;
    if (WATERMARK(size, capacity) >= WATERMARK_HIGH)
    {
        capacity += 1;
        do
        {
            capacity *= 2;
        } while (WATERMARK(size, capacity) >= WATERMARK_LOW);

        if (!RESULT_IS_OK(vector_try_fit(vec, capacity)))
        {
            return RESULT_ERR(1);
        }
    }

    vec->items[vec->size++] = mk_vector_item_(value, cleanup);

    return RESULT_OK(0);
}

void vector_fit(BORROWED vector_t * vec, COPIED uint64_t new_cap)
{
    COPIED result_t result = vector_try_fit(vec, new_cap);
    if (RESULT_IS_OK(result))
    {
        return;
    }

    uint64_t errcode = (uint64_t) result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): NIL vector argument.", __func__);
        } break;

        case 1:
        {
            PANIC("%s(): the old capacity is %lu greater than or equal to the new capacity is %lu.",
                __func__,
                vec->capacity,
                new_cap);
        } break;

        default:
        {
            PANIC("%s(): Unknown error: %lu.", __func__, errcode);
        } break;
    }
}

COPIED result_t vector_try_fit(BORROWED vector_t * vec, COPIED uint64_t new_cap)
{
    if (!vec)
    {
        return RESULT_ERR(0);
    }

    uint64_t old_cap = vec->capacity;
    if (old_cap >= new_cap)
    {
        return RESULT_ERR(1);
    }

    vec->items    = realloc_smart(vec->items, new_cap * sizeof(vector_item_p));
    vec->capacity = new_cap;

    return RESULT_OK(0);
}

COPIED uint64_t vector_get_size(BORROWED vector_t * vec)
{
    SCP(vec);
    return vec->size;
}

COPIED result_t vector_try_get_size(BORROWED vector_t * vec)
{
    if (!vec)
    {
        return RESULT_ERR(0);
    }
    return RESULT_OK(vec->size);
}

COPIED uint64_t vector_get_capacity(BORROWED vector_t * vec)
{
    SCP(vec);
    return vec->capacity;
}

COPIED result_t vector_try_get_capacity(BORROWED vector_t * vec)
{
    if (!vec)
    {
        return RESULT_ERR(0);
    }
    return RESULT_OK(vec->capacity);
}

COPIED bool vector_is_empty(BORROWED vector_t * vec)
{
    return (vec)
        ? EQ(vec->size, 0)
        : true;
}

COPIED void * vector_dispose(OWNED void * arg)
{
    if (!arg)
    {
        return NIL;
    }

    OWNED vector_t * vec = CAST(arg, vector_t*);

    uint64_t size = vec->size;
    for (uint64_t i = 0; i < size; i++)
    {
        vector_item_dispose_(vec->items[i]);
    }

    free_smart(vec->items);
    return dispose(vec);
}
