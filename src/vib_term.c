#include "vib_term.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>

#include "common.h"

/* ─────────────────────────────────────────────────────────────────────────────
 * ANSI Escape Sequences
 * ───────────────────────────────────────────────────────────────────────────── */

#define ESC                 "\x1b"
#define CSI                 ESC "["

#define SEQ_CLEAR_SCREEN    CSI "2J"
#define SEQ_CURSOR_HOME     CSI "H"
#define SEQ_CURSOR_HIDE     CSI "?25l"
#define SEQ_CURSOR_SHOW     CSI "?25h"

/* ─────────────────────────────────────────────────────────────────────────────
 * Module State
 * ───────────────────────────────────────────────────────────────────────────── */

static struct {
    struct termios original;    /* Original terminal attributes */
    int rows;                   /* Terminal rows */
    int cols;                   /* Terminal columns */
    bool raw;                   /* True if raw mode is active */
    volatile sig_atomic_t resized;  /* Resize flag (signal-safe) */
} _term_state = {
    .rows = 24,
    .cols = 80,
    .initialized = false,
    .resized = 0,
};

/* ─────────────────────────────────────────────────────────────────────────────
 * Forward Declarations
 * ───────────────────────────────────────────────────────────────────────────── */

static void term_enter_raw_mode_();
static void term_leave_raw_mode_();
static void term_setup_raw_mode_signals_();
static void term_default_signal_handler_(int sig);
static void term_winch_handler_(int sig);
static void term_query_size_(void);

/* ─────────────────────────────────────────────────────────────────────────────
 * Lifecycle
 * ───────────────────────────────────────────────────────────────────────────── */

COPIED result_t vib_term_init()
{
    TODO;
}

void vib_term_quit()
{
    if (!_term_state.raw)
    {
        return;
    }

    TODO;
    // ...

    _term_state.raw = false;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Raw Mode
 * ───────────────────────────────────────────────────────────────────────────── */

static void term_enter_raw_mode_()
{
}

static void term_leave_raw_mode_()
{
}



