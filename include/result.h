#pragma once

#include "common.h"
#include "memory.h"
#include "assertion.h"

#define RESULT_IS_OK(result)            ((result).tag == T_RESULT_SUCCESS)
#define RESULT_IS_ERR(result)           ((result).tag == T_RESULT_FAILURE)
#define OPTION_IS_SOME(option)          ((option).tag == T_OPTION_SOME)
#define OPTION_IS_NONE(option)          ((option).tag == T_OPTION_NONE)

#define RESULT_OK(value)                (mk_result_ok(CAST((value), uintptr_t)))
#define RESULT_ERR(value)               (mk_result_err(CAST((value), uintptr_t)))

#define OPTION_SOME(value)              (mk_option_some(CAST((value), uintptr_t)))
#define OPTION_NONE()                   ((option_t) { .tag = T_OPTION_NONE })

#define OPTIONP_SOME(value)             (mk_optionp_some(CAST((value), uintptr_t)))
#define OPTIONP_NONE()                  (mk_optionp_none())

#define RESULT_UNWRAP(result)           (result_unwrap(result, NIL))
#define RESULT_UNWRAP_ALT(result, alt)  (result_unwrap_alt(result, CAST(alt, uintptr_t)))

#define OPTION_UNWRAP(option)           (option_unwrap(option, NIL))
#define OPTION_UNWRAP_ALT(option, alt)  (option_unwrap_alt(option, CAST(alt, uintptr_t)))

typedef enum result_tag_t result_tag_t;
typedef enum option_tag_t option_tag_t;
typedef struct result_t result_t;
typedef struct option_t option_t;

typedef result_t * result_p;
typedef option_t * option_p;

typedef COPIED OWNED BORROWED uintptr_t result_callback_fn(COPIED OWNED BORROWED uintptr_t);
typedef COPIED OWNED BORROWED uintptr_t option_callback_fn();

enum result_tag_t
{
    T_RESULT_FAILURE = 0,
    T_RESULT_SUCCESS = 1,
};

struct result_t
{
    COPIED result_tag_t tag;
    union
    {
        COPIED OWNED BORROWED uintptr_t ok;
        COPIED OWNED BORROWED uintptr_t err;
    };
};

COPIED result_t mk_result(COPIED result_tag_t tag, COPIED OWNED BORROWED uintptr_t value);
COPIED result_t mk_result_ok(COPIED OWNED BORROWED uintptr_t ok);
COPIED result_t mk_result_err(COPIED OWNED BORROWED uintptr_t err);

COPIED OWNED BORROWED uintptr_t result_unwrap(COPIED result_t result, BORROWED result_callback_fn * onerr);
COPIED OWNED BORROWED uintptr_t result_unwrap_alt(COPIED result_t result, COPIED OWNED BORROWED uintptr_t alternative);

enum option_tag_t
{
    T_OPTION_NONE = 0,
    T_OPTION_SOME = 1,
};

struct option_t
{
    COPIED                option_tag_t tag;
    COPIED OWNED BORROWED uintptr_t    some;
};

COPIED option_t mk_option(COPIED option_tag_t tag, COPIED OWNED BORROWED uintptr_t value);
COPIED option_t mk_option_some(COPIED OWNED BORROWED uintptr_t value);

COPIED OWNED BORROWED uintptr_t option_unwrap(COPIED option_t option, BORROWED option_callback_fn * onnone);
COPIED OWNED BORROWED uintptr_t option_unwrap_alt(COPIED option_t option, COPIED OWNED BORROWED uintptr_t alternative);

OWNED option_t * mk_optionp(COPIED option_tag_t tag, COPIED OWNED BORROWED uintptr_t value);
OWNED option_t * mk_optionp_some(COPIED OWNED BORROWED uintptr_t value);
OWNED option_t * mk_optionp_none();
