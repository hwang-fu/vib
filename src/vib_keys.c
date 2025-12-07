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
static COPIED vib_key_t parse_escape_sequence_();
static COPIED vib_key_t parse_csi_sequence_(BORROWED const unsigned char * seq, COPIED int len);
static COPIED vib_key_t parse_ss3_sequence_(BORROWED const unsigned char * seq, COPIED int len);

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
