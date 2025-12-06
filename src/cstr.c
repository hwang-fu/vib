#include "cstr.h"

COPIED bool strcmp_smart_ignorecase(BORROWED const char * s1, BORROWED const char * s2)
{
    /// 1. if points to the same memory / both @const {NIL}, definitely same.
    if (EQ(s1, s2))
    {
        return True;
    }

    /// 2. if only one string is @const {NIL}, definitely not same.
    if (!s1 || !s2)
    {
        return False;
    }

    while (*s1 && *s2 && EQ(cto_english_lowerletter(*s1), cto_english_lowerletter(*s2)))
    {
        s1++;
        s2++;
    }
    return EQ(cto_english_lowerletter(*s1), cto_english_lowerletter(*s2));
}

COPIED bool strncmp_smart_ignorecase(BORROWED const char * s1, BORROWED const char * s2, COPIED uint64_t length)
{
    /// 1. if length is @const {0}, no need to compare.
    if (EQ(length, 0))
    {
        return True;
    }

    /// 2. if points to the same memory / both @const {NIL}, definitely same.
    if (EQ(s1, s2))
    {
        return True;
    }

    /// 3. if only one string is @const {NIL}, definitely not same.
    if (!s1 || !s2)
    {
        return False;
    }

    while (--length && *s1 && *s2 && EQ(cto_english_lowerletter(*s1), cto_english_lowerletter(*s2)))
    {
        s1++;
        s2++;
    }
    return EQ(cto_english_lowerletter(*s1), cto_english_lowerletter(*s2));
}

COPIED bool strcmp_smart(BORROWED const char * s1, BORROWED const char * s2)
{
#ifdef CSTR_IGNORE_CASE
    return strcmp_smart_ignorecase(s1, s2);
#else
    /// 1. if points to the same memory / both @const {NIL}, definitely same.
    if (EQ(s1, s2))
    {
        return True;
    }

    /// 2. if only one string is @const {NIL}, definitely not same.
    if (!s1 || !s2)
    {
        return False;
    }

    return EQ(0, strcmp(s1, s2));
#endif // CSTR_IGNORE_CASE
}

COPIED bool strncmp_smart(BORROWED const char * s1, BORROWED const char * s2, COPIED uint64_t length)
{
#ifdef CSTR_IGNORE_CASE
    return strncmp_smart_ignorecase(s1, s2, length);
#else
    /// 1. if length is @const {0}, no need to compare.
    if (EQ(length, 0))
    {
        return True;
    }

    /// 2. if points to the same memory / both @const {NIL}, definitely same.
    if (EQ(s1, s2))
    {
        return True;
    }

    /// 3. if only one string is @const {NIL}, definitely not same.
    if (!s1 || !s2)
    {
        return False;
    }

    return EQ(0, strncmp(s1, s2, length));
#endif // CSTR_IGNORE_CASE
}

OWNED char * strdup_smart(BORROWED const char * s)
{
    if (EQ(s, NIL))
    {
        s = "";
    }
    OWNED char * theString = strdup(s);
    if (!theString)
    {
        PANIC("@%s(): failed to duplicate string \"%s\"", __func__, s);
    }
    return theString;
}

COPIED bool cstr_starts_with_owned(BORROWED const char * s, OWNED char * prefix)
{
    bool result = cstr_starts_with(s, prefix);
    free_smart(prefix);
    return result;
}

COPIED bool cstr_starts_with_ignorecase(BORROWED const char * s, BORROWED const char * prefix)
{
    uint64_t len1 = strlen_smart(s);
    uint64_t len2 = strlen_smart(prefix);

    // if len(s) < len(prefix), then definitely @const {False}.
    if (len1 < len2)
    {
        return False;
    }

    // @const {""} is the prefix for any string.
    if (prefix && EQ(len2, 0))
    {
        return True;
    }

    return strncmp_smart_ignorecase(s, prefix, len2);
}

COPIED bool cstr_starts_with(BORROWED const char * s, BORROWED const char * prefix)
{
    uint64_t len1 = strlen_smart(s);
    uint64_t len2 = strlen_smart(prefix);

    // if len(s) < len(prefix), then definitely @const {False}.
    if (len1 < len2)
    {
        return False;
    }

    // @const {""} is the prefix for any string.
    if (prefix && EQ(len2, 0))
    {
        return True;
    }

    return strncmp_smart(s, prefix, len2);
}

COPIED bool cstr_ends_with_owned(BORROWED const char * s, OWNED char * suffix)
{
    bool result = cstr_ends_with(s, suffix);
    free_smart(suffix);
    return result;
}

COPIED bool cstr_ends_with_ignorecase(BORROWED const char * s, BORROWED const char * suffix)
{
    uint64_t len1 = strlen_smart(s);
    uint64_t len2 = strlen_smart(suffix);

    // if len(s) < len(suffix), then definitely @const {False}.
    if (len1 < len2)
    {
        return False;
    }

    // @const {""} is the suffix for any string.
    if (suffix && EQ(len2, 0))
    {
        return True;
    }

    uint64_t offset = len1 - len2;
    return strcmp_smart_ignorecase(s + offset, suffix);
}

COPIED bool cstr_ends_with(BORROWED const char * s, BORROWED const char * suffix)
{
    uint64_t len1 = strlen_smart(s);
    uint64_t len2 = strlen_smart(suffix);

    // if len(s) < len(suffix), then definitely @const {False}.
    if (len1 < len2)
    {
        return False;
    }

    // @const {""} is the suffix for any string.
    if (suffix && EQ(len2, 0))
    {
        return True;
    }

    uint64_t offset = len1 - len2;
    return strcmp_smart(s + offset, suffix);
}

COPIED char cstr_at(BORROWED const char * s, uint64_t idx)
{
    if (strlen_smart(s) <= idx)
    {
        PANIC("%s(): index %lu out of range for \"%s\".",
                __func__, idx, s);
    }
    return s[idx];
}

OWNED char * mk_cstr(BORROWED const char * s1, BORROWED const char * s2)
{
    const uint64_t len1 = strlen_smart(s1);
    const uint64_t len2 = strlen_smart(s2);
    const uint64_t total = len1 + len2;

    if (EQ(total, 0))
    {
        return strdup_smart("");
    }

    if (EQ(len1, 0))
    {
        return strdup_smart(s2);
    }

    if (EQ(len2, 0))
    {
        return strdup_smart(s1);
    }

    OWNED char * s = new((total + 1) * sizeof(char));
    memcpy(s,        s1, len1);
    memcpy(s + len1, s2, len2);
    s[total] = '\0';

    return s;
}

OWNED char * mk_cstr_owned(OWNED char * s1, OWNED char * s2)
{
    OWNED char * theString = mk_cstr(s1, s2);
    free_smart(s1);
    free_smart(s2);
    return theString;
}

OWNED char * mk_cstr_with_owned_prefix(BORROWED const char * s, OWNED char * prefix)
{
    OWNED char * theString = mk_cstr(prefix, s);
    free_smart(prefix);
    return theString;
}

OWNED char * mk_cstr_with_owned_suffix(BORROWED const char * s, OWNED char * suffix)
{
    OWNED char * theString = mk_cstr(s, suffix);
    free_smart(suffix);
    return theString;
}

OWNED char * mk_cstr_tolower_owned(OWNED char * s)
{
    if (!s)
    {
        return strdup_smart("");
    }

    uint64_t len = strlen_smart(s);
    for (uint64_t i = 0; i < len; i++)
    {
        s[i] = cto_english_lowerletter(s[i]);
    }
    return s;
}

OWNED char * mk_cstr_tolower(BORROWED const char * s)
{
    if (!s)
    {
        return strdup_smart("");
    }

    uint64_t len = strlen_smart(s);
    OWNED char * theString = new((len + 1) * sizeof(char));
    for (uint64_t i = 0; i < len; i++)
    {
        theString[i] = cto_english_lowerletter(s[i]);
    }
    theString[len] = '\0';
    return theString;
}

OWNED char * mk_cstr_toupper_owned(OWNED char * s)
{
    if (!s)
    {
        return strdup_smart("");
    }

    uint64_t len = strlen_smart(s);
    for (uint64_t i = 0; i < len; i++)
    {
        s[i] = cto_english_upperletter(s[i]);
    }
    return s;
}

OWNED char * mk_cstr_toupper(BORROWED const char * s)
{
    if (!s)
    {
        return strdup_smart("");
    }

    uint64_t len = strlen_smart(s);
    OWNED char * theString = new((len + 1) * sizeof(char));
    for (uint64_t i = 0; i < len; i++)
    {
        theString[i] = cto_english_upperletter(s[i]);
    }
    theString[len] = '\0';
    return theString;
}

OWNED char * mk_cstr_from_char(COPIED const char c)
{
    char theString[2] = { 0 };
    theString[0] = c;
    return strdup_smart(theString);
}

OWNED char * mk_cstr_from_u8_buffer(BORROWED const uint8_t * buffer, const uint64_t size)
{
    OWNED char * theString = new((size + 1) * sizeof(char));
    memcpy(theString, buffer, size);
    theString[size] = '\0';
    return theString;
}

OWNED char * mk_cstr_from_owned_u8_buffer(OWNED uint8_t * buffer, const uint64_t size)
{
    OWNED char * theString = mk_cstr_from_u8_buffer(buffer, size);
    free_smart(buffer);
    return theString;
}

OWNED char * strrev_smart(BORROWED const char * s)
{
    return strrev_smart_owned(strdup_smart(s));
}

OWNED char * strrev_smart_owned(OWNED char * s)
{
    if (EQ(s, NIL))
    {
        return strdup_smart("");
    }

    uint64_t len = strlen_smart(s);
    if (EQ(len, 0))
    {
        return s;
    }

    for (uint64_t i = 0; i < len/2; i++)
    {
        char temp = s[i];
        s[i] = s[len - 1 - i];
        s[len - 1 - i] = temp;
    }

    return s;
}
