#pragma once

#include "common.h"

typedef int32_t vib_key_t;

#define VIB_KEY_NONE        (-1)
#define VIB_KEY_UNKNOWN     (-2)

#define VIB_KEY_TAB         (0x09)  /* CTRL + I */
#define VIB_KEY_ENTER       (0x0d)  /* CTRL + M */
#define VIB_KEY_BACKSPACE   (0x7f)

#define VIB_CTRL            (0x1000)
#define VIB_ALT             (0x2000)
#define VIB_SHIFT           (0x4000)
#define VIB_MASK            (VIB_CTRL | VIB_ALT | VIB_SHIFT)

/* Special keys: 256+ to avoid ASCII collision */
enum {
    VIB_KEY_ESC = 256,

    /* Navigation Keys */
    VIB_KEY_UP,
    VIB_KEY_DOWN,
    VIB_KEY_LEFT,
    VIB_KEY_RIGHT,
    VIB_KEY_HOME,
    VIB_KEY_END,
    VIB_KEY_PAGE_UP,
    VIB_KEY_PAGE_DOWN,

    /* Edit Keys */
    VIB_KEY_INSERT,
    VIB_KEY_DELETE,

    /* Function Keys */
    VIB_KEY_F1,
    VIB_KEY_F2,
    VIB_KEY_F3,
    VIB_KEY_F4,
    VIB_KEY_F5,
    VIB_KEY_F6,
    VIB_KEY_F7,
    VIB_KEY_F8,
    VIB_KEY_F9,
    VIB_KEY_F10,
    VIB_KEY_F11,
    VIB_KEY_F12,
};

COPIED vib_key_t vib_keys_read();

COPIED bool vib_key_combo_ctrl(COPIED vib_key_t key);
COPIED bool vib_key_combo_alt(COPIED vib_key_t key);
COPIED bool vib_key_combo_shift(COPIED vib_key_t key);
COPIED bool vib_key_combo(COPIED vib_key_t key);

/* Get human-readable name for a key (for debugging) */
BORROWED const char * vib_key_name_get(COPIED vib_key_t key);

