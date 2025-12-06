#pragma once

#include <string.h>

#include "common.h"
#include "assertion.h"
#include "result.h"


#define cis_digit(c)  (('0' <= (c)) && ((c) <= '9'))
#define cis_bdigit(c) (EQ((c), '0') || EQ((c), '1'))
#define cis_odigit(c) (('0' <= (c)) && ((c) <= '7'))
#define cis_xdigit(c) (cis_digit(c) || (('a' <= (c)) && ((c) <= 'f')) || (('A' <= (c)) && ((c) <= 'F')))

#define cis_english_upperletter(c) (('A' <= (c)) && ((c) <= 'Z'))
#define cis_english_lowerletter(c) (('a' <= (c)) && ((c) <= 'z'))
#define cis_english_letter(c) (cis_english_upperletter(c) || cis_english_lowerletter(c))

#define cto_english_upperletter(c) (cis_english_lowerletter(c) ? ((c) - 'a' + 'A') : (c))
#define cto_english_lowerletter(c) (cis_english_upperletter(c) ? ((c) - 'A' + 'a') : (c))

/**
 * @brief       Returns @const {0} if @param {s} is @const {NIL}
 */
#define strlen_smart(s) (EQ(s, NIL) ? 0 : ((uint64_t) strlen(s)))

/**
 * @brief       Smartly compares two C strings.
 *
 * Performs a {NIL}-safe comparison between @param s1 and @param {s2}.
 * - Returns @const {True} if both strings have the length @const {0}.
 * - Otherwise, compares the string contents using strcmp().
 *
 * Optionally supports case-insensitive comparison if `CSTR_IGNORE_CASE` macro is defined at compile time.
 *
 * @param s1 Borrowed pointer to the first C string (may be @const {NIL}).
 * @param s2 Borrowed pointer to the second C string (may be @const {NIL}).
 *
 * @return @const {True}  if both strings are @const {NIL} or have identical contents.
 * @return @const {False} if only one is @const {NIL} or the strings differ.
 */
COPIED bool strcmp_smart(BORROWED const char * s1, BORROWED const char * s2);

/**
 * @brief       If @param {length} is @const {0}, the function always returns @const {True}.
 */
COPIED bool strncmp_smart(BORROWED const char * s1, BORROWED const char * s2, COPIED uint64_t length);

/**
 * @brief       Compares two C strings ignoring case. Other from that, similar to @func {strcmp_smart}.
 */
COPIED bool strcmp_smart_ignorecase(BORROWED const char * s1, BORROWED const char * s2);

/**
 * @brief       If @param {length} is @const {0}, the function always returns @const {True}.
 */
COPIED bool strncmp_smart_ignorecase(BORROWED const char * s1, BORROWED const char * s2, COPIED uint64_t length);

/**
 * @brief       Smartly duplicates a C string.
 *
 * Creates a dynamically allocated duplicate of @param {s}.
 * - If @param s is @const {NIL}, returns a newly allocated empty string @const {""}.
 * - The result is always heap-allocated and never @const {NIL}.
 * - The caller MUST release the returned string with @func {free}.
 *
 * @param s Borrowed pointer to the source C string (may be @const {NIL}).
 * @return OWNED pointer to a heap-allocated copy of the string.
 *         Guaranteed non-{NIL} and safe to pass to @func {free}.
 */
OWNED char * strdup_smart(BORROWED const char * s);

/**
 * @brief       If both inputs are of length @const {0}, it also returns @const {True}.
 */
COPIED bool cstr_starts_with_ignorecase(BORROWED const char * s, BORROWED const char * prefix);

/**
 * @brief       If both inputs are of length @const {0}, it also returns @const {True}.
 */
COPIED bool cstr_ends_with_ignorecase(BORROWED const char * s, BORROWED const char * suffix);

/**
 * @brief       If both inputs are of length @const {0}, it also returns @const {True}.
 */
COPIED bool cstr_starts_with(BORROWED const char * s, BORROWED const char * prefix);

/**
 * @brief       If both inputs are of length @const {0}, it also returns @const {True}.
 */
COPIED bool cstr_ends_with(BORROWED const char * s, BORROWED const char * suffix);

/**
 * @brief       If both inputs are of length @const {0}, it also returns @const {True}.
 */
COPIED bool cstr_starts_with_owned(BORROWED const char * s, OWNED char * prefix);

/**
 * @brief       If both inputs are of length @const {0}, it also returns @const {True}.
 */
COPIED bool cstr_ends_with_owned(BORROWED const char * s, OWNED char * suffix);

COPIED char cstr_at(BORROWED const char * s, COPIED uint64_t idx);

OWNED char * mk_cstr(BORROWED const char * s1, BORROWED const char * s2);
OWNED char * mk_cstr_owned(OWNED char * s1, OWNED char * s2);
OWNED char * mk_cstr_with_owned_suffix(BORROWED const char * s, OWNED char * suffix);
OWNED char * mk_cstr_with_owned_prefix(BORROWED const char * s, OWNED char * prefix);
OWNED char * mk_cstr_toupper(BORROWED const char * s);
OWNED char * mk_cstr_toupper_owned(OWNED char * s);
OWNED char * mk_cstr_tolower(BORROWED const char * s);
OWNED char * mk_cstr_tolower_owned(OWNED char * s);
OWNED char * mk_cstr_from_char(COPIED const char c);
OWNED char * mk_cstr_from_u8_buffer(BORROWED const uint8_t * buffer, COPIED const uint64_t size);
OWNED char * mk_cstr_from_owned_u8_buffer(OWNED uint8_t * buffer, COPIED const uint64_t size);

OWNED char * strrev_smart(BORROWED const char * s);
OWNED char * strrev_smart_owned(OWNED char * s);
