#include "vib_keys.h"

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#include "vib_term.h"

/* ─────────────────────────────────────────────────────────────────────────────
 * Constants
 * ───────────────────────────────────────────────────────────────────────────── */
#define ESC                 (0x1b)
#define ESC_TIMEOUT_DS      (1) /* deciseconds (100ms) */
#define MAX_SEQUENCE_LENGTH (8)


/* ─────────────────────────────────────────────────────────────────────────────
 * Forward Declarations
 * ───────────────────────────────────────────────────────────────────────────── */

static void set_read_timeout_(COPIED bool enable);
static COPIED int32_t read_byte_timeout_();
static COPIED vib_key_t parse_esc_sequence_();
static COPIED vib_key_t parse_csi_sequence_(BORROWED const unsigned char * seq, COPIED uint8_t len);
static COPIED vib_key_t parse_ss3_sequence_(BORROWED const unsigned char * seq, COPIED uint8_t len);

/* ─────────────────────────────────────────────────────────────────────────────
 * Timeout Mode Control
 * ───────────────────────────────────────────────────────────────────────────── */

static struct termios _saved;
static bool _timeout_saved = false;

static void set_read_timeout_(COPIED bool enable)
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);

    if (!_timeout_saved)
    {
        _saved         = CLONE(term);
        _timeout_saved = true;
    }

    if (enable)
    {
        /* enabling */
        term.c_cc[VMIN]  = 0;
        term.c_cc[VTIME] = ESC_TIMEOUT_DS;
    }
    else
    {
        /* disabling */
        term.c_cc[VMIN]  = 1;
        term.c_cc[VTIME] = 0;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

static COPIED int32_t read_byte_timeout_()
{
    unsigned char c;
    return (1 == read(STDIN_FILENO, &c, 1)) ? c : -1;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Escape Sequence Parsing
 * ───────────────────────────────────────────────────────────────────────────── */

static COPIED vib_key_t parse_esc_sequence_()
{
}

static COPIED vib_key_t parse_csi_sequence_(BORROWED const unsigned char * seq, COPIED uint8_t len)
{
}

static COPIED vib_key_t parse_ss3_sequence_(BORROWED const unsigned char * seq, COPIED uint8_t len)
{
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Public API
 * ───────────────────────────────────────────────────────────────────────────── */

COPIED vib_key_t vib_keys_read()
{
}

BORROWED const char * vib_key_name_get(COPIED vib_key_t key)
{
    COPIED char keyname[64] = { 0 };

    COPIED char prefix[7] = { 0 };
    if (vib_key_combo_ctrl(key))
    {
        strcat(prefix, "C-");
    }
    if(vib_key_combo_alt(key))
    {
        strcat(prefix, "M-");
    }
    if (vib_key_combo_shift(key))
    {
        strcat(prefix, "S-");
    }

    COPIED vib_key_t base = vib_key_base(key);

    const char * name = NIL;
    switch (base)
    {
        case VIB_KEY_NONE:          name = "None";      break;
        case VIB_KEY_UNKNOWN:       name = "Unknown";   break;
        case VIB_KEY_ESC:           name = "ESC";       break;
        case VIB_KEY_UP:            name = "Up";        break;
        case VIB_KEY_DOWN:          name = "Down";      break;
        case VIB_KEY_LEFT:          name = "Left";      break;
        case VIB_KEY_RIGHT:         name = "Right";     break;
        case VIB_KEY_HOME:          name = "Home";      break;
        case VIB_KEY_END:           name = "End";       break;
        case VIB_KEY_PAGE_UP:       name = "PageUp";    break;
        case VIB_KEY_PAGE_DOWN:     name = "PageDown";  break;
        case VIB_KEY_INSERT:        name = "Insert";    break;
        case VIB_KEY_DELETE:        name = "Delete";    break;
        case VIB_KEY_F1:            name = "F1";        break;
        case VIB_KEY_F2:            name = "F2";        break;
        case VIB_KEY_F3:            name = "F3";        break;
        case VIB_KEY_F4:            name = "F4";        break;
        case VIB_KEY_F5:            name = "F5";        break;
        case VIB_KEY_F6:            name = "F6";        break;
        case VIB_KEY_F7:            name = "F7";        break;
        case VIB_KEY_F8:            name = "F8";        break;
        case VIB_KEY_F9:            name = "F9";        break;
        case VIB_KEY_F10:           name = "F10";       break;
        case VIB_KEY_F11:           name = "F11";       break;
        case VIB_KEY_F12:           name = "F12";       break;
        case VIB_KEY_TAB:           name = "Tab";       break;
        case VIB_KEY_ENTER:         name = "Enter";     break;
        case VIB_KEY_BACKSPACE:     name = "Backspace"; break;
    }

    if (name)
    {
        snprintf(keyname, sizeof(keyname), "%s%s", prefix, name);
    }
    else if (0x20 <= base && base < 0x7f)
    {
        /* printable ASCII */
        snprintf(keyname, sizeof(keyname), "%s'%c", prefix, (char) base);
    }
    else
    {
        /* non-printable */
        snprintf(keyname, sizeof(keyname), "%s0x%02X", prefix, base);
    }

    return keyname;
}

