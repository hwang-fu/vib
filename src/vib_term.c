#include "vib_term.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>

#include "common.h"

#define VIB_TERM_DEFAULT_ROWS    (24)
#define VIB_TERM_DEFAULT_COLUMNS (80)

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
    int columns;                /* Terminal columns */
    bool raw;                   /* True if raw mode is active */
    volatile sig_atomic_t resized;  /* Resize flag (signal-safe) */
} _term_state = {
    .rows = VIB_TERM_DEFAULT_ROWS,
    .columns = VIB_TERM_DEFAULT_COLUMNS,
    .raw = false,
    .resized = 0,
};

/* ─────────────────────────────────────────────────────────────────────────────
 * Forward Declarations
 * ───────────────────────────────────────────────────────────────────────────── */

static void term_enter_raw_mode_();
static void term_leave_raw_mode_();
static void term_setup_raw_mode_signals_();
static void term_default_signal_handler_(int sig);
static void term_sig_winch_handler_(int sig);
static void term_query_size_(void);

/* ─────────────────────────────────────────────────────────────────────────────
 * Lifecycle
 * ───────────────────────────────────────────────────────────────────────────── */

COPIED result_t vib_term_init()
{
    if (_term_state.raw)
    {
        return RESULT_OK(1);
    }

    /* must be a terminal */
    if (0 == isatty(STDERR_FILENO))
    {
        return RESULT_ERR(1);
    }

    /* save original terminal attributes */
    if (-1 == tcgetattr(STDIN_FILENO, &_term_state.original))
    {
        return RESULT_ERR(2);
    }

    term_enter_raw_mode_();
    TODO; // ...

    atexit(vib_term_quit);

    return RESULT_OK(0);
}

void vib_term_quit()
{
    if (!_term_state.raw)
    {
        return;
    }

    TODO;
    // ...
    term_leave_raw_mode_();
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
    _term_state.raw = true;
}

static void term_leave_raw_mode_()
{
    if (!_term_state.raw)
    {
        return;
    }
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &_term_state.original);
    _term_state.raw = false;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Signal Handling
 * ───────────────────────────────────────────────────────────────────────────── */

static void term_setup_raw_mode_signals_()
{
    struct sigaction sa = { 0 };

    sa.sa_handler = term_default_signal_handler_;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NIL);
    sigaction(SIGTERM, &sa, NIL);

    /* Handler for SIGWINCH — window resize */
    sa.sa_handler = term_sig_winch_handler_;
    sa.sa_flags = SA_RESTART; // restart interrupted syscalls
    sigaction(SIGWINCH, &sa, NIL);
}

static void term_default_signal_handler_(int sig)
{
    (void) sig;
    vib_term_quit();
    _exit(128 + sig);
}

static void term_sig_winch_handler_(int sig)
{
    (void) sig;
    _term_state.resized = 1;
}


