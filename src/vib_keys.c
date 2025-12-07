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

static void enable_read_timeout_();
static void disable_read_timeout_();
static COPIED vib_key_t parse_esc_sequence_();
static COPIED vib_key_t parse_csi_sequence_();
static COPIED vib_key_t parse_csi_arrow_sequence_(COPIED int32_t key);
static COPIED vib_key_t parse_csi_extended_sequence_(COPIED int32_t key);
static COPIED vib_key_t parse_ss3_sequence_();
static COPIED vib_key_t parse_ctrl_sequence_(BORROWED const int32_t key);
static COPIED vib_key_t parse_alt_ctrl_sequence_(BORROWED const int32_t key);

/* ─────────────────────────────────────────────────────────────────────────────
 * Timeout Mode Control
 * ───────────────────────────────────────────────────────────────────────────── */

static void enable_read_timeout_()
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_cc[VMIN]  = 0;
    term.c_cc[VTIME] = ESC_TIMEOUT_DS;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

static void disable_read_timeout_()
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_cc[VMIN]  = 1;
    term.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Escape Sequence Parsing
 * ───────────────────────────────────────────────────────────────────────────── */

static COPIED vib_key_t parse_csi_extended_sequence_(COPIED int32_t key)
{
    int32_t n = key - '0';

    int32_t b = vib_terminal_read_raw_byte();
    if (b == -1)
    {
        disable_read_timeout_();
        return VIB_KEY_UNKNOWN;
    }

    // also accept two digit number
    if ('0' <= b && b <= '9')
    {
        n = (n * 10) + (b - '0');
        b = vib_terminal_read_raw_byte();
        if (b == -1)
        {
            disable_read_timeout_();
            return VIB_KEY_UNKNOWN;
        }
    }
    disable_read_timeout_();

    if (b != '~')
    {
        return VIB_KEY_UNKNOWN;
    }

    switch (n)
    {
        case 1:     return VIB_KEY_HOME;
        case 2:     return VIB_KEY_INSERT;
        case 3:     return VIB_KEY_DELETE;
        case 4:     return VIB_KEY_END;
        case 5:     return VIB_KEY_PAGE_UP;
        case 6:     return VIB_KEY_PAGE_DOWN;
        case 11:    return VIB_KEY_F1;
        case 12:    return VIB_KEY_F2;
        case 13:    return VIB_KEY_F3;
        case 14:    return VIB_KEY_F4;
        case 15:    return VIB_KEY_F5;
        case 17:    return VIB_KEY_F6;
        case 18:    return VIB_KEY_F7;
        case 19:    return VIB_KEY_F8;
        case 20:    return VIB_KEY_F9;
        case 21:    return VIB_KEY_F10;
        case 23:    return VIB_KEY_F11;
        case 24:    return VIB_KEY_F12;
        default:    return VIB_KEY_UNKNOWN;
    }
}

static COPIED vib_key_t parse_csi_arrow_sequence_(COPIED int32_t key)
{
    switch (key)
    {
        case 'A': return VIB_KEY_UP;
        case 'B': return VIB_KEY_DOWN;
        case 'C': return VIB_KEY_RIGHT;
        case 'D': return VIB_KEY_LEFT;

        // should never be happening, in the future consider panicing it.
        default : return VIB_KEY_UNKNOWN;
    }
}

static COPIED vib_key_t parse_csi_sequence_()
{
    int32_t b = vib_terminal_read_raw_byte();
    if (b == -1)
    {
        disable_read_timeout_();
        return VIB_KEY_UNKNOWN;
    }

    if ('A' <= b && b <= 'D')
    {
        disable_read_timeout_();
        return parse_csi_arrow_sequence_(b);
    }

    // ESC [ H
    if (b == 'H')
    {
        disable_read_timeout_();
        return VIB_KEY_HOME;
    }

    // ESC [ F
    if (b == 'F')
    {
        disable_read_timeout_();
        return VIB_KEY_END;
    }

    // extended sequences:
    // ESC [ <number> ~
    if ('0' <= b && b <= '9')
    {
        return parse_csi_extended_sequence_(b);
    }

    return VIB_KEY_UNKNOWN;
}

static COPIED vib_key_t parse_ss3_sequence_()
{
    int32_t key = vib_terminal_read_raw_byte();
    disable_read_timeout_();

    if (key == -1)
    {
        return VIB_KEY_UNKNOWN;
    }

    switch (key)
    {
        case 'P': key = VIB_KEY_F1;         break;
        case 'Q': key = VIB_KEY_F2;         break;
        case 'R': key = VIB_KEY_F3;         break;
        case 'S': key = VIB_KEY_F4;         break;
        case 'H': key = VIB_KEY_HOME;       break;
        case 'F': key = VIB_KEY_END;        break;
        /* Some terminals also send these for arrows */
        case 'A': key = VIB_KEY_UP;         break;
        case 'B': key = VIB_KEY_DOWN;       break;
        case 'C': key = VIB_KEY_RIGHT;      break;
        case 'D': key = VIB_KEY_LEFT;       break;
        default : key = VIB_KEY_UNKNOWN;    break;
    }
    return key;
}

static COPIED vib_key_t parse_esc_sequence_()
{
    /* Enable timeout mode for reading escape sequence */
    enable_read_timeout_();

    int32_t b = vib_terminal_read_raw_byte(); // read first byte after ESC
    if (b == -1)
    {
        /* Timeout: ESC was pressed alone */
        disable_read_timeout_();
        return VIB_KEY_ESC;
    }

    /* ALT + key => ESC followed by printable ASCII */
    if (0x20 <= b && b < 0x7f && b != '[' && b != 'O')
    {
        disable_read_timeout_();
        return (VIB_ALT | b);
    }

    /* ALT + CTRL + key => ESC followed by control character */
    if (0x01 <= b && b <= 0x1a)
    {
        disable_read_timeout_();
        return parse_alt_ctrl_sequence_(b);
    }

    /* CSI sequence => ESC [ ... */
    if (b == '[')
    {
        return parse_csi_sequence_();
    }

    /* SS3 sequence => ESC O ... */
    if (b == 'O')
    {
        return parse_ss3_sequence_();
    }

    disable_read_timeout_();
    return VIB_KEY_UNKNOWN;
}

static COPIED vib_key_t parse_ctrl_sequence_(BORROWED const int32_t key)
{
    // TAB      => 9
    // CTRL + L => 9
    //
    // ENTER    => 13
    // CTRL + M => 13
    // ...
    // Sadly you can't distinguish them, they are the same byte at the terminal level,
    // so we will just ignore those ctrl combos and treat them as the special sequences.
    if (key == 9 || key == 13)
    {
        return key;
    }

    return (VIB_CTRL | (key - 1 + 'a'));
}

static COPIED vib_key_t parse_alt_ctrl_sequence_(BORROWED const int32_t key)
{
    // TAB      => 9
    // CTRL + L => 9
    //
    // ENTER    => 13
    // CTRL + M => 13
    // ...
    // Sadly you can't distinguish them, they are the same byte at the terminal level,
    // so we will just ignore those ctrl combos and treat them as the special sequences.
    if (key == 9 || key == 13)
    {
        return key;
    }

    return (VIB_ALT | VIB_CTRL | (key - 1 + 'a'));
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Public API
 * ───────────────────────────────────────────────────────────────────────────── */

COPIED vib_key_t vib_keys_read()
{
    int32_t b = vib_terminal_read_raw_byte();
    if (b == -1)
    {
        return VIB_KEY_NONE;
    }

    if (b == ESC)
    {
        return parse_esc_sequence_();
    }

    if (0x01 <= b && b <= 0x1a)
    {
        return parse_ctrl_sequence_(b);
    }

    return b;
}

BORROWED const char * vib_key_name_get(COPIED vib_key_t key)
{
    static COPIED char keyname[64] = { 0 };

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
        snprintf(keyname, sizeof(keyname), "%s'%c'", prefix, (char) base);
    }
    else
    {
        /* non-printable */
        snprintf(keyname, sizeof(keyname), "%s0x%02X", prefix, base);
    }

    return keyname;
}

