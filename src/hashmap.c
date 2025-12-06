#include "hashmap.h"

struct hashmap_entry_t
{
    OWNED                 char            * key;
    COPIED OWNED BORROWED uintptr_t         val;
    OWNED                 hashmap_entry_t * next;
};

static uint64_t fnv1a_hash_(BORROWED const char * key);

static OWNED hashmap_entry_t * mk_hme_(BORROWED const char * key, COPIED OWNED BORROWED uintptr_t val);
static void hm_ins_helper_(BORROWED hashmap_entry_t ** entries, COPIED uint64_t idx, OWNED hashmap_entry_t * entry);
static COPIED void * hme_dispose_(OWNED void * arg, BORROWED dispose_fn * cleanup);
static COPIED void * hme_dispose_recursive_(OWNED void * arg, BORROWED dispose_fn * cleanup);

static COPIED uint64_t fnv1a_hash_(BORROWED const char * key)
{
    SCP(key);
    COPIED uint64_t hash = 14695981039346656037UL;  // FNV offset basis
    for (; *key; )
    {
        hash ^= (uint64_t)(*key++);
        hash *= 1099511628211UL;                    // FNV prime
    }
    return hash;
}

OWNED hashmap_t * hm_init(OWNED hashmap_t * hm, COPIED uint64_t capacity, BORROWED dispose_fn * cleanup)
{
    if (!hm)
    {
        hm = new(sizeof(hashmap_t));
    }

    if (EQ(capacity, 0))
    {
        capacity = HASHMAP_DEFAULT_CAPACITY;
        WARNINGF("%s(): hashmap initialized capacity is zero, default to %lu.",
                __func__,
                HASHMAP_DEFAULT_CAPACITY);
    }

    hm->capacity = capacity;
    hm->size     = 0UL;
    hm->entries  = zeros(capacity * sizeof(hashmap_entry_t*));
    hm->dispose  = cleanup;

    return hm;
}

/*
 * Possible overloads:
 * @li OWNED hashmap_t * mk_hm(HASHMAP_INIT_DEFAULT);
 * @li OWNED hashmap_t * mk_hm(HASHMAP_INIT_CAPACITY_ONLY, COPIED uint64_t capacity)
 * @li OWNED hashmap_t * mk_hm(HASHMAP_INIT_DISPOSE_ONLY, BORROWED dispose_fn * cleanup)
 * @li OWNED hashmap_t * mk_hm(HASHMAP_INIT_CAPACITY_DISPOSE, COPIED uint64_t capacity, BORROWED dispose_fn * cleanup)
 * @li OWNED hashmap_t * mk_hm(HASHMAP_INIT_DISPOSE_CAPACITY, BORROWED dispose_fn * cleanup, COPIED uint64_t capacity)
 */
OWNED hashmap_t * mk_hm(hashmap_init_t mode, ...)
{
    va_list ap;
    va_start(ap, mode);

    uint64_t     capacity = HASHMAP_DEFAULT_CAPACITY;
    dispose_fn * cleanup  = NIL;
    switch (mode)
    {
        case HASHMAP_INIT_DEFAULT:
        {
        } break;

        case HASHMAP_INIT_CAPACITY_ONLY:
        {
            capacity = va_arg(ap, uint64_t);
        } break;

        case HASHMAP_INIT_DISPOSE_ONLY:
        {
            cleanup = va_arg(ap, dispose_fn*);
        } break;

        case HASHMAP_INIT_CAPACITY_DISPOSE:
        {
            capacity = va_arg(ap, uint64_t);
            cleanup  = va_arg(ap, dispose_fn*);
        } break;

        case HASHMAP_INIT_DISPOSE_CAPACITY:
        {
            cleanup  = va_arg(ap, dispose_fn*);
            capacity = va_arg(ap, uint64_t);
        } break;

        default:
        {
            PANIC("%s(): unkown hashmap init mode %d", __func__, mode);
        } break;
    }

    va_end(ap);
    return hm_init(NIL, capacity, cleanup);
}

static void hm_ins_helper_(BORROWED hashmap_entry_t ** entries, COPIED uint64_t idx, OWNED hashmap_entry_t * entry)
{
    if (!entries)
    {
        PANIC("%s(): NIL entries", __func__);
    }

    if (!entry)
    {
        PANIC("%s(): NIL entry", __func__);
    }

    entry->next = entries[idx];
    entries[idx] = entry;
}

void _hm_ins(BORROWED hashmap_t * hm, BORROWED const char * key, COPIED OWNED BORROWED uintptr_t val)
{
    COPIED result_t result = _hm_try_ins(hm, key, val);
    if (RESULT_IS_OK(result))
    {
        return;
    }

    uint64_t errcode = result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): hm argument is " CRAYON_TO_BOLD("NIL") ".", __func__);
        } break;

        case 1:
        {
            PANIC("%s(): key is " CRAYON_TO_BOLD("NIL") ".", __func__);
        } break;

        case 2:
        {
            PANIC("%s(): key is " CRAYON_TO_BOLD("\"\"") ".", __func__);
        } break;

        case 3:
        {
            PANIC("%s(): failed to fit the capacity.", __func__);
        } break;

        case 4:
        {
            PANIC("%s(): Key " CRAYON_TO_BOLD("\"%s\"") " already exists.", __func__, key);
        } break;

        default:
        {
            PANIC("%s(): unknown error code %lu.", __func__, errcode);
        } break;
    }
}

void _hm_ins_with_owned_key(BORROWED hashmap_t * hm, OWNED char * key, COPIED OWNED BORROWED uintptr_t val)
{
    _hm_ins(hm, key, val);
    free_smart(key);
}

OWNED option_t * _hm_set(BORROWED hashmap_t * hm, BORROWED const char * key, COPIED OWNED BORROWED uintptr_t val)
{
    COPIED result_t result = _hm_try_set(hm, key, val);
    if (RESULT_IS_OK(result))
    {
        return (option_t *) RESULT_UNWRAP(result);
    }

    uint64_t errcode = result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): hm argument is " CRAYON_TO_BOLD("NIL") ".", __func__);
        } break;

        case 1:
        {
            PANIC("%s(): key is " CRAYON_TO_BOLD("NIL") ".", __func__);
        } break;

        case 2:
        {
            PANIC("%s(): key is " CRAYON_TO_BOLD("\"\"") ".", __func__);
        } break;

        case 3:
        {
            PANIC("%s(): failed to fit the capacity.", __func__);
        } break;

        default:
        {
            PANIC("%s(): unknown error code %lu.", __func__, errcode);
        } break;
    }
    return NIL; // unreachable, satisfies compiler
}

OWNED option_t * _hm_set_with_owned_key(BORROWED hashmap_t * hm, OWNED char * key, COPIED OWNED BORROWED uintptr_t val)
{
    OWNED option_t * option = _hm_set(hm, key, val);
    free_smart(key);
    return option;
}

uintptr_t hm_get(BORROWED hashmap_t * hm, BORROWED const char * key)
{
    COPIED result_t result = hm_try_get(hm, key);
    if (RESULT_IS_OK(result))
    {
        return RESULT_UNWRAP(result);
    }

    uint64_t errcode = result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): hm argument is " CRAYON_TO_BOLD("NIL") ".", __func__);
        } break;

        case 1:
        {
            PANIC("%s(): key is " CRAYON_TO_BOLD("NIL") ".", __func__);
        } break;

        case 2:
        {
            PANIC("%s(): key is " CRAYON_TO_BOLD("\"\"") ".", __func__);
        } break;

        case 3:
        {
            PANIC("%s(): hm size is " CRAYON_TO_BOLD("0") ".", __func__);
        } break;

        case 4:
        {
            PANIC("%s(): Key " CRAYON_TO_BOLD("\"%s\"") " does not exist.", __func__, key);
        } break;

        default:
        {
            PANIC("%s(): unknown error code %lu.", __func__, errcode);
        } break;
    }
    return 0; // unreachable, satisfies compiler
}

uintptr_t hm_get_with_owned_key(BORROWED hashmap_t * hm, OWNED char * key)
{
    COPIED result_t result = hm_try_get(hm, key);
    if (RESULT_IS_OK(result))
    {
        free_smart(key);
        return RESULT_UNWRAP(result);
    }

    uint64_t errcode = result.err;
    switch (errcode)
    {
        case 0:
        {
            free_smart(key);
            PANIC("%s(): hm argument is " CRAYON_TO_BOLD("NIL") ".", __func__);
        } break;

        case 1:
        {
            free_smart(key);
            PANIC("%s(): key is " CRAYON_TO_BOLD("NIL") ".", __func__);
        } break;

        case 2:
        {
            free_smart(key);
            PANIC("%s(): key is " CRAYON_TO_BOLD("\"\"") ".", __func__);
        } break;

        case 3:
        {
            free_smart(key);
            PANIC("%s(): hm size is " CRAYON_TO_BOLD("0") ".", __func__);
        } break;

        case 4:
        {
            ERRORF("%s(): Key " CRAYON_TO_BOLD("\"%s\"") " does not exist.", __func__, key);
            free_smart(key);
            exit(EXIT_FAILURE);
        } break;

        default:
        {
            PANIC("%s(): unknown error code %lu.", __func__, errcode);
        } break;
    }
    return 0; // unreachable, satisfies compiler
}

void hm_del(BORROWED hashmap_t * hm, BORROWED const char * key)
{

    COPIED result_t result = hm_try_del(hm, key);
    if (RESULT_IS_OK(result))
    {
        return;
    }

    uint64_t errcode = result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): hm argument is " CRAYON_TO_BOLD("NIL") ".", __func__);
        } break;

        case 1:
        {
            PANIC("%s(): key is " CRAYON_TO_BOLD("NIL") ".", __func__);
        } break;

        case 2:
        {
            PANIC("%s(): key is " CRAYON_TO_BOLD("\"\"") ".", __func__);
        } break;

        case 3:
        {
            PANIC("%s(): hm size is " CRAYON_TO_BOLD("0") ".", __func__);
        } break;

        case 4:
        {
            PANIC("%s(): Key " CRAYON_TO_BOLD("\"%s\"") " does not exist.", __func__, key);
        } break;

        default:
        {
            PANIC("%s(): unknown error code %lu.", __func__, errcode);
        } break;
    }
}

void hm_del_with_owned_key(BORROWED hashmap_t * hm, OWNED char * key)
{
    hm_del(hm, key);
    free_smart(key);
}

COPIED result_t hm_try_get(BORROWED hashmap_t * hm, BORROWED const char * key)
{
    if (!hm)
    {
        return RESULT_ERR(0);
    }

    if (!key)
    {
        return RESULT_ERR(1);
    }

    if (EQ(strlen_smart(key), 0))
    {
        return RESULT_ERR(2);
    }

    if (EQ(hm->size, 0))
    {
        return RESULT_ERR(3);
    }

    uint64_t h   = fnv1a_hash_(key);
    uint64_t cap = hm->capacity;
    uint64_t idx = h % cap;

    BORROWED hashmap_entry_t * entry = hm->entries[idx];
    while (entry)
    {
        if (strcmp_smart(key, entry->key))
        {
            return RESULT_OK(entry->val);
        }
        entry = entry->next;
    }

    return RESULT_ERR(4);
}

COPIED result_t hm_try_get_with_owned_key(BORROWED hashmap_t * hm, OWNED char * key)
{
    COPIED result_t result = hm_try_get(hm, key);
    free_smart(key);
    return result;
}

COPIED result_t _hm_try_ins(BORROWED hashmap_t * hm, BORROWED const char * key, COPIED OWNED BORROWED uintptr_t val)
{
    if (!hm)
    {
        return RESULT_ERR(0);
    }

    if (!key)
    {
        return RESULT_ERR(1);
    }

    if (EQ(strlen_smart(key), 0))
    {
        return RESULT_ERR(2);
    }

    uint64_t sz  = hm->size;
    uint64_t cap = hm->capacity;
    if (WATERMARK(sz, cap) >= WATERMARK_HIGH)
    {
        cap += 1;
        do
        {
            cap *= 2;
        } while (WATERMARK(sz, cap) >= WATERMARK_LOW);

        if (!RESULT_IS_OK(hm_try_fit(hm, cap)))
        {
            return RESULT_ERR(3);
        }
    }

    uint64_t h   = fnv1a_hash_(key);
    uint64_t idx = h % hm->capacity;

    // Making sure that there is no duplicate key.
    if (hm->entries[idx])
    {
        BORROWED hashmap_entry_t * entry = hm->entries[idx];
        while (entry)
        {
            if (strcmp_smart(key, entry->key))
            {
                return RESULT_ERR(4);
            }
            entry = entry->next;
        }
    }

    hm_ins_helper_(hm->entries, idx, mk_hme_(key, val));

    hm->size += 1;

    return RESULT_OK(0);
}

COPIED result_t _hm_try_ins_with_owned_key(BORROWED hashmap_t * hm, OWNED char * key, COPIED OWNED BORROWED uintptr_t val)
{
    COPIED result_t result = _hm_try_ins(hm, key, val);
    free_smart(key);
    return result;
}

COPIED result_t _hm_try_set(BORROWED hashmap_t * hm, BORROWED const char * key, COPIED OWNED BORROWED uintptr_t val)
{
    if (!hm)
    {
        return RESULT_ERR(0);
    }

    if (!key)
    {
        return RESULT_ERR(1);
    }

    if (EQ(strlen_smart(key), 0))
    {
        return RESULT_ERR(2);
    }

    uint64_t size = hm->size;
    uint64_t cap  = hm->capacity;
    if (WATERMARK(size, cap) >= WATERMARK_HIGH)
    {
        cap += 1;
        do
        {
            cap *= 2;
        } while (WATERMARK(size, cap) >= WATERMARK_LOW);

        if (!RESULT_IS_OK(hm_try_fit(hm, cap)))
        {
            return RESULT_ERR(3);
        }
    }

    uint64_t h   = fnv1a_hash_(key);
    uint64_t idx = h % hm->capacity;

    // Check whether there is a duplicate key or not.
    if (hm->entries[idx])
    {
        BORROWED hashmap_entry_t * entries = hm->entries[idx];
        while (entries)
        {
            if (strcmp_smart(key, entries->key))
            {
                uintptr_t old_val = entries->val;
                entries->val = val;
                return RESULT_OK(OPTIONP_SOME(old_val));
            }
            entries = entries->next;
        }
    }

    hm_ins_helper_(hm->entries, idx, mk_hme_(key, val));

    hm->size += 1;

    // Even though here it returns as ERR, but it is used as to distinguish between replacing and inserting.
    return RESULT_OK(OPTIONP_NONE());
}

COPIED result_t _hm_try_set_with_owned_key(BORROWED hashmap_t * hm, OWNED char * key, COPIED OWNED BORROWED uintptr_t val)
{
    COPIED result_t result = _hm_try_set(hm, key, val);
    free_smart(key);
    return result;
}

COPIED result_t hm_try_del(BORROWED hashmap_t * hm, BORROWED const char * key)
{
    if (!hm)
    {
        return RESULT_ERR(0);
    }

    if (!key)
    {
        return RESULT_ERR(1);
    }

    if (EQ(strlen_smart(key), 0))
    {
        return RESULT_ERR(2);
    }

    if (EQ(hm->size, 0))
    {
        return RESULT_ERR(3);
    }

    uint64_t h   = fnv1a_hash_(key);
    uint64_t cap = hm->capacity;
    uint64_t idx = h % cap;

    BORROWED hashmap_entry_t * entry = hm->entries[idx];
    BORROWED hashmap_entry_t * prev  = NIL;
    while (entry)
    {
        if (strcmp_smart(key, entry->key))
        {
            if (EQ(prev, NIL))
            {
                hm->entries[idx] = entry->next;
            }
            else
            {
                prev->next = entry->next;
            }
            hme_dispose_(entry, hm->dispose);
            hm->size -= 1;
            return RESULT_OK(0);
        }
        prev  = entry;
        entry = entry->next;
    }

    return RESULT_ERR(4);
}

COPIED result_t hm_try_del_with_owned_key(BORROWED hashmap_t * hm, OWNED char * key)
{
    COPIED result_t result = hm_try_del(hm, key);
    free_smart(key);
    return result;
}

bool hm_has(BORROWED hashmap_t * hm, BORROWED const char * key)
{
    COPIED result_t result = hm_try_get(hm, key);
    if (RESULT_IS_OK(result))
    {
        return true;
    }
    return false;
}

bool hm_has_with_owned_key(BORROWED hashmap_t * hm, OWNED char * key)
{
    COPIED result_t result = hm_try_get_with_owned_key(hm, key);
    if (RESULT_IS_OK(result))
    {
        return true;
    }
    return false;
}

uint64_t hm_get_size(BORROWED hashmap_t * hm)
{
    COPIED result_t result = hm_try_get_size(hm);
    if (RESULT_IS_OK(result))
    {
        return RESULT_UNWRAP(result);
    }

    uint64_t errcode = (uint64_t) result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): NIL hm argument.", __func__);
        } break;

        default:
        {
            PANIC("%s(): unknown error: %d.", __func__, errcode);
        } break;
    }
    return 0; // unreachable, satisfies compiler
}

COPIED result_t hm_try_get_size(BORROWED hashmap_t * hm)
{
    if (!hm)
    {
        return RESULT_ERR(0);
    }
    return RESULT_OK(hm->size);
}

COPIED uint64_t hm_get_capacity(BORROWED hashmap_t * hm)
{
    COPIED result_t result = hm_try_get_capacity(hm);
    if (RESULT_IS_OK(result))
    {
        return RESULT_UNWRAP(result);
    }

    uint64_t errcode = (uint64_t) result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): NIL hm argument.", __func__);
        } break;

        default:
        {
            PANIC("%s(): unknown error: %d.", __func__, errcode);
        } break;
    }
    return 0; // unreachable, satisfies compiler
}

COPIED result_t hm_try_get_capacity(BORROWED hashmap_t * hm)
{
    if (!hm)
    {
        return RESULT_ERR(0);
    }

    return RESULT_OK(hm->capacity);
}

void hm_fit(BORROWED hashmap_t * hm, const uint64_t new_cap)
{
    COPIED result_t result = hm_try_fit(hm, new_cap);
    if (RESULT_IS_OK(result))
    {

        return;
    }

    uint64_t errcode = result.err;
    switch (errcode)
    {
        case 0:
        {
            PANIC("%s(): hm argument is " CRAYON_TO_BOLD("NIL") ".", __func__);
        } break;

        case 1:
        {
            PANIC("%s(): old capacity"
                    " "
                    CRAYON_TO_BOLD("%lu")
                    " "
                    "is greater than or equal to the new capacity"
                    " "
                    CRAYON_TO_BOLD("%lu")
                    ".",
                    __func__,
                    hm->capacity,
                    new_cap);
        } break;

        default:
        {
            PANIC("%s(): unknown error code %lu.", __func__, errcode);
        } break;
    }
}

COPIED result_t hm_try_fit(BORROWED hashmap_t * hm, const uint64_t new_cap)
{
    if (!hm)
    {
        return RESULT_ERR(0);
    }

    uint64_t old_cap = hm->capacity;
    if (old_cap >= new_cap)
    {
        return RESULT_ERR(1);
    }

    OWNED hashmap_entry_t ** new_entries = zeros(new_cap * sizeof(hashmap_entry_t*));
    OWNED hashmap_entry_t ** old_entries = hm->entries;

    for (uint64_t i = 0; i < old_cap; i++)
    {
        hashmap_entry_t * entries = old_entries[i];
        while (entries)
        {
            OWNED hashmap_entry_t * entry = entries;
            entries                       = entries->next;
            entry->next                   = NIL;

            uint64_t idx = fnv1a_hash_(entry->key) % new_cap;

            hm_ins_helper_(new_entries, idx, entry);
        }
    }

    free_smart(old_entries);
    hm->entries  = new_entries;
    hm->capacity = new_cap;

    return RESULT_OK(0);
}

static OWNED hashmap_entry_t * mk_hme_(BORROWED const char * key, COPIED OWNED BORROWED uintptr_t val)
{
    OWNED hashmap_entry_t * hme = new(sizeof(hashmap_entry_t));
    hme->key                    = strdup_smart(key);
    hme->val                    = val;
    hme->next                   = NIL;
    return hme;
}

static COPIED void * hme_dispose_(OWNED void * arg, dispose_fn * cleanup)
{
    if (!arg)
    {
        return NIL;
    }

    OWNED hashmap_entry_t * hme = CAST(arg, hashmap_entry_t*);

    free_smart(hme->key);
    if (cleanup)
    {
        cleanup(CAST(hme->val, void*));
    }

    return dispose(hme);
}

static COPIED void * hme_dispose_recursive_(OWNED void * arg, dispose_fn * cleanup)
{
    if (!arg)
    {
        return NIL;
    }

    OWNED hashmap_entry_t * hme = CAST(arg, hashmap_entry_t*);

    if (hme->next)
    {
        hme_dispose_recursive_(hme->next, cleanup);
    }

    return hme_dispose_(hme, cleanup);
}

COPIED void * hm_dispose(OWNED void * arg)
{
    if (!arg)
    {
        return NIL;
    }

    OWNED hashmap_t * hm = CAST(arg, hashmap_t*);

    uint64_t     capacity = hm->capacity;
    dispose_fn * cleanup  = hm->dispose;
    for (uint64_t i = 0; i < capacity; i++)
    {
        hme_dispose_recursive_(hm->entries[i], cleanup);
    }
    free_smart(hm->entries);

    return dispose(hm);
}
