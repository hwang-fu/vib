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
    if (_term_state.raw)
    {
        return;
    }

    struct termios term = CLONE(_term_state.original);

    /*
     * Input flags:
     * - IXON: Disable Ctrl-S/Ctrl-Q flow control
     * - ICRNL: Disable CR to NL translation
     * - BRKINT: Disable break condition signals
     * - INPCK: Disable parity checking
     * - ISTRIP: Disable stripping of 8th bit
     */
    term.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);

    /*
     * Output flags:
     * - OPOST: Disable output processing
     */
    term.c_oflag &= ~(OPOST);

    /*
     * Control flags:
     * - CS8: Set character size to 8 bits
     */
    term.c_cflag |= (CS8);

    /*
     * Local flags:
     * - ECHO: Disable echo
     * - ICANON: Disable canonical mode (line buffering)
     * - ISIG: Disable Ctrl-C/Ctrl-Z signals
     * - IEXTEN: Disable Ctrl-V
     */
    term.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

    /*
     * Control characters:
     * - VMIN: Minimum bytes for read (1 = blocking read)
     * - VTIME: Timeout (0 = no timeout)
     */
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

static void term_leave_raw_mode_()
{
    if (!_term_state.raw)
    {
        return;
    }
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &_term_state.original);
}



