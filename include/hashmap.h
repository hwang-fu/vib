#pragma once

#include <stdarg.h>

#include "common.h"
#include "assertion.h"
#include "result.h"
#include "crayon.h"
#include "cstr.h"

#ifndef HASHMAP_DEFAULT_CAPACITY
#define HASHMAP_DEFAULT_CAPACITY (20)
#endif // HASHMAP_DEFAULT_CAPACITY

#define hm_ins(hm, key, val)                    _hm_ins(hm, key, CAST(val, uintptr_t))
#define hm_ins_with_owned_key(hm, key, val)     _hm_ins_with_owned_key(hm, key, CAST(val, uintptr_t))
#define hm_try_ins(hm, key, val)                _hm_try_ins(hm, key, CAST(val, uintptr_t))
#define hm_try_ins_with_owned_key(hm, key, val) _hm_try_ins_with_owned_key(hm, key, CAST(val, uintptr_t))

#define hm_set(hm, key, val)                    _hm_set(hm, key, CAST(val, uintptr_t))
#define hm_set_with_owned_key(hm, key, val)     _hm_set_with_owned_key(hm, key, CAST(val, uintptr_t))
#define hm_try_set(hm, key, val)                _hm_try_set(hm, key, CAST(val, uintptr_t))
#define hm_try_set_with_owned_key(hm, key, val) _hm_try_set_with_owned_key(hm, key, CAST(val, uintptr_t))

typedef struct hashmap_t hashmap_t;
typedef struct hashmap_entry_t hashmap_entry_t;

typedef enum hashmap_init_t
{
    HASHMAP_INIT_DEFAULT = 0,
    HASHMAP_INIT_CAPACITY_ONLY,
    HASHMAP_INIT_DISPOSE_ONLY,
    HASHMAP_INIT_CAPACITY_DISPOSE,
    HASHMAP_INIT_DISPOSE_CAPACITY,
} hashmap_init_t;

struct hashmap_t
{
    COPIED   uint64_t           capacity    ;
    COPIED   uint64_t           size        ;
    OWNED    hashmap_entry_t ** entries     ;
    BORROWED dispose_fn      *  dispose     ;
};

OWNED hashmap_t * hm_init(OWNED hashmap_t * hm, COPIED uint64_t capacity, BORROWED dispose_fn * cleanup);

/*
 * Possible overloads:
 * @li OWNED hashmap_t * mk_hm(HASHMAP_INIT_DEFAULT);
 * @li OWNED hashmap_t * mk_hm(HASHMAP_INIT_CAPACITY_ONLY, COPIED uint64_t capacity)
 * @li OWNED hashmap_t * mk_hm(HASHMAP_INIT_DISPOSE_ONLY, BORROWED dispose_fn * cleanup)
 * @li OWNED hashmap_t * mk_hm(HASHMAP_INIT_CAPACITY_DISPOSE, COPIED uint64_t capacity, BORROWED dispose_fn * cleanup)
 * @li OWNED hashmap_t * mk_hm(HASHMAP_INIT_DISPOSE_CAPACITY, BORROWED dispose_fn * cleanup, COPIED uint64_t capacity)
 */
OWNED hashmap_t * mk_hm(COPIED hashmap_init_t mode, ...);

void _hm_ins(BORROWED hashmap_t * hm, BORROWED const char * key, COPIED OWNED BORROWED uintptr_t val);

void _hm_ins_with_owned_key(BORROWED hashmap_t * hm, OWNED char * key, COPIED OWNED BORROWED uintptr_t val);

OWNED option_t * _hm_set(BORROWED hashmap_t * hm, BORROWED const char * key, COPIED OWNED BORROWED uintptr_t val);

OWNED option_t * _hm_set_with_owned_key(BORROWED hashmap_t * hm, OWNED char * key, COPIED OWNED BORROWED uintptr_t val);

COPIED OWNED BORROWED uintptr_t hm_get(BORROWED hashmap_t * hm, BORROWED const char * key);

COPIED OWNED BORROWED uintptr_t hm_get_with_owned_key(BORROWED hashmap_t * hm, OWNED char * key);

void hm_del(BORROWED hashmap_t * hm, BORROWED const char * key);

void hm_del_with_owned_key(BORROWED hashmap_t * hm, OWNED char * key);

COPIED result_t hm_try_get(BORROWED hashmap_t * hm, BORROWED const char * key);

COPIED result_t hm_try_get_with_owned_key(BORROWED hashmap_t * hm, OWNED char * key);

COPIED result_t _hm_try_ins(BORROWED hashmap_t * hm, BORROWED const char * key, COPIED OWNED BORROWED uintptr_t val);

COPIED result_t _hm_try_ins_with_owned_key(BORROWED hashmap_t * hm, OWNED char * key, COPIED OWNED BORROWED uintptr_t val);

COPIED result_t _hm_try_set(BORROWED hashmap_t * hm, BORROWED const char * key, COPIED OWNED BORROWED uintptr_t val);

COPIED result_t _hm_try_set_with_owned_key(BORROWED hashmap_t * hm, OWNED char * key, COPIED OWNED BORROWED uintptr_t val);

COPIED result_t hm_try_del(BORROWED hashmap_t * hm, BORROWED const char * key);

COPIED result_t hm_try_del_with_owned_key(BORROWED hashmap_t * hm, OWNED char * key);

COPIED bool hm_has(BORROWED hashmap_t * hm, BORROWED const char * key);

COPIED bool hm_has_with_owned_key(BORROWED hashmap_t * hm, OWNED char * key);

COPIED uint64_t hm_get_size(BORROWED hashmap_t * hm);

void hm_fit(BORROWED hashmap_t * hm, COPIED const uint64_t new_cap);

COPIED result_t hm_try_fit(BORROWED hashmap_t * hm, COPIED const uint64_t new_cap);

COPIED result_t hm_try_get_size(BORROWED hashmap_t * hm);

COPIED uint64_t hm_get_capacity(BORROWED hashmap_t * hm);

COPIED result_t hm_try_get_capacity(BORROWED hashmap_t * hm);

COPIED void * hm_dispose(OWNED void * arg);
