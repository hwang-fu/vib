#include "result.h"

static COPIED OWNED BORROWED uintptr_t result_onerr_default_(COPIED OWNED BORROWED uintptr_t err)
{
    PANIC("Unwrapping a err-ed result with err: %lu", err);
    return err; // unreachable, just here to match the return type.
}

static COPIED OWNED BORROWED uintptr_t option_onnone_default_()
{
    PANIC("Unwrapping an option with nothing");
    return 0; // unreachable, just here to match the return type.
}

COPIED result_t mk_result(COPIED result_tag_t tag, COPIED OWNED BORROWED uintptr_t value)
{
    if (eq(tag, T_RESULT_SUCCESS))
    {
        return mk_result_ok(value);
    }
    else
    {
        return mk_result_err(value);
    }
}

COPIED result_t mk_result_ok(COPIED OWNED BORROWED uintptr_t ok)
{
    return (result_t) { .tag = T_RESULT_SUCCESS, .ok = ok };
}

COPIED result_t mk_result_err(COPIED OWNED BORROWED uintptr_t err)
{
    return (result_t) { .tag = T_RESULT_FAILURE, .err = err };
}

COPIED OWNED BORROWED uintptr_t result_unwrap(COPIED result_t result, BORROWED result_callback_fn * onerr)
{
    if (!onerr)
    {
        onerr = result_onerr_default_;
    }

    if (result.tag)
    {
        return result.ok;
    }
    else
    {
        return onerr(result.err);
    }
}

COPIED OWNED BORROWED uintptr_t result_unwrap_alt(COPIED result_t result, COPIED OWNED BORROWED uintptr_t alternative)
{
    if (result.tag)
    {
        return result.ok;
    }
    else
    {
        return alternative;
    }
}

COPIED option_t mk_option(COPIED option_tag_t tag, COPIED OWNED BORROWED uintptr_t value)
{
    if (tag == T_OPTION_SOME)
    {
        return mk_option_some(value);
    }
    else
    {
        return OPTION_NONE();
    }
}

COPIED option_t mk_option_some(COPIED OWNED BORROWED uintptr_t value)
{
    return (option_t) { .tag = T_OPTION_SOME, .some = value };
}

COPIED OWNED BORROWED uintptr_t option_unwrap(COPIED option_t option, BORROWED option_callback_fn * onnone)
{
    if (!onnone)
    {
        onnone = option_onnone_default_;
    }

    if (option.tag == T_OPTION_SOME)
    {
        return option.some;
    }
    else
    {
        return onnone();
    }
}

COPIED OWNED BORROWED uintptr_t option_unwrap_alt(COPIED option_t option, COPIED OWNED BORROWED uintptr_t alternative)
{
    if (option.tag == T_OPTION_SOME)
    {
        return option.some;
    }
    else
    {
        return alternative;
    }
}

OWNED option_t * mk_optionp(COPIED option_tag_t tag, COPIED OWNED BORROWED uintptr_t value)
{
    if (tag == T_OPTION_SOME)
    {
        return mk_optionp_some(value);
    }
    else
    {
        return mk_optionp_none();
    }
}

OWNED option_t * mk_optionp_some(COPIED OWNED BORROWED uintptr_t value)
{
    OWNED option_t * option = new(sizeof(option_t));
    option->tag             = T_OPTION_SOME;
    option->some            = value;
    return option;
}

OWNED option_t * mk_optionp_none()
{
    OWNED option_t * option = new(sizeof(option_t));
    option->tag             = T_OPTION_NONE;
    return option;
}
