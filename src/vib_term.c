#include "vib_term.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>

#include "common.h"

#define VIB_TERMINAL_DEFAULT_ROWS    (24UL)
#define VIB_TERMINAL_DEFAULT_COLUMNS (80UL)

/* ─────────────────────────────────────────────────────────────────────────────
 * ANSI Escape Sequences
 * ───────────────────────────────────────────────────────────────────────────── */

#define ESC                     "\x1b"
#define CSI                     ESC "["

#define VIB_NORMAL_BUFFER       (CSI "?1049l")      // switch to normal buffer
#define VIB_ALTERNATE_BUFFER    (CSI "?1049h")      // switch to alternate buffer

#define VIB_CURSOR_HOME         (CSI "H")           // move cursor to (1,1) which is the top-left corner
#define VIB_CURSOR_HIDE         (CSI "?25l")        // hide cursor
#define VIB_CURSOR_SHOW         (CSI "?25h")        // show cursor
#define VIB_CURSOR_LOCATION     (CSI "%lu;%luH")    // move cursor to (row, column)

#define VIB_TERMINAL_CLR        (CSI "2J")          // clear the entire terminal screen

/* ─────────────────────────────────────────────────────────────────────────────
 * Module State
 * ───────────────────────────────────────────────────────────────────────────── */

static struct {
    COPIED struct termios original;    /* Original terminal attributes */
    COPIED uint64_t rows;                   /* Terminal rows */
    COPIED uint64_t columns;                /* Terminal columns */
    COPIED bool raw;                   /* True if raw mode is active */
    COPIED bool alt;                    /* True if alternate buffer is active */
    COPIED volatile sig_atomic_t resized;  /* Resize flag (signal-safe) */
} _terminal_state = {
    .rows    = VIB_TERMINAL_DEFAULT_ROWS,
    .columns = VIB_TERMINAL_DEFAULT_COLUMNS,
    .raw     = false,
    .alt     = false,
    .resized = 0,
};

/* ─────────────────────────────────────────────────────────────────────────────
 * Forward Declarations
 * ───────────────────────────────────────────────────────────────────────────── */

static void terminal_enter_raw_mode_();
static void terminal_leave_raw_mode_();
static void terminal_setup_raw_mode_signals_();
static void terminal_sig_default_handler_(int sig);
static void terminal_sig_winch_handler_(int sig);
static void terminal_size_query_();

/* ─────────────────────────────────────────────────────────────────────────────
 * Lifecycle
 * ───────────────────────────────────────────────────────────────────────────── */

COPIED result_t vib_terminal_init()
{
    if (_terminal_state.raw)
    {
        return RESULT_OK(1);
    }

    /* must be a terminal */
    if (0 == isatty(STDERR_FILENO))
    {
        return RESULT_ERR(1);
    }

    /* save original terminal attributes */
    if (-1 == tcgetattr(STDIN_FILENO, &_terminal_state.original))
    {
        return RESULT_ERR(2);
    }

    terminal_enter_raw_mode_();
    terminal_setup_raw_mode_signals_();
    terminal_size_query_();

    atexit(vib_terminal_quit);

    return RESULT_OK(0);
}

void vib_terminal_quit()
{
    if (!_terminal_state.raw)
    {
        return;
    }

    vib_terminal_cursor_show();
    vib_terminal_clear();
    vib_terminal_cursor_home();

    terminal_leave_raw_mode_();
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Raw Mode
 * ───────────────────────────────────────────────────────────────────────────── */

static void terminal_enter_raw_mode_()
{
    if (_terminal_state.raw)
    {
        return;
    }

    struct termios term = CLONE(_terminal_state.original);

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
    _terminal_state.raw = true;
}

static void terminal_leave_raw_mode_()
{
    if (!_terminal_state.raw)
    {
        return;
    }
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &_terminal_state.original);
    _terminal_state.raw = false;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Signal Handling
 * ───────────────────────────────────────────────────────────────────────────── */

static void terminal_setup_raw_mode_signals_()
{
    struct sigaction sa = { 0 };

    sa.sa_handler = terminal_sig_default_handler_;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NIL);
    sigaction(SIGTERM, &sa, NIL);

    /* Handler for SIGWINCH — window resize */
    sa.sa_handler = terminal_sig_winch_handler_;
    sa.sa_flags = SA_RESTART; // restart interrupted syscalls
    sigaction(SIGWINCH, &sa, NIL);
}

static void terminal_sig_default_handler_(int sig)
{
    (void) sig;
    vib_terminal_quit();
    _exit(128 + sig);
}

static void terminal_sig_winch_handler_(int sig)
{
    (void) sig;
    _terminal_state.resized = 1;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Terminal Size
 * ───────────────────────────────────────────────────────────────────────────── */

static void terminal_size_query_()
{
    struct winsize ws;
    if (0 == ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws))
    {
        _terminal_state.rows    = ws.ws_row;
        _terminal_state.columns = ws.ws_col;
    }
}

COPIED uint64_t vib_terminal_get_rows()
{
    return _terminal_state.rows;
}

COPIED uint64_t vib_terminal_get_columns()
{
    return _terminal_state.columns;
}

void vib_terminal_size_update()
{
    terminal_size_query_();
}

COPIED bool vib_terminal_was_resized(void)
{
    if (_terminal_state.resized)
    {
        _terminal_state.resized = 0;
        terminal_size_query_();
        return true;
    }
    return false;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Terminal Screen (TUI) Operations
 * ───────────────────────────────────────────────────────────────────────────── */

void vib_tui_use_normal_buffer()
{
    if (_terminal_state.alt)
    {
        vib_terminal_write(VIB_NORMAL_BUFFER, sizeof(VIB_NORMAL_BUFFER) - 1);
        _terminal_state.alt = false;
    }
}

void vib_tui_use_alternate_buffer()
{
    if (!_terminal_state.alt)
    {
        vib_terminal_write(VIB_ALTERNATE_BUFFER, sizeof(VIB_ALTERNATE_BUFFER) - 1);
        _terminal_state.alt = true;
    }
}

void vib_tui_toggle_buffer()
{
    if (_terminal_state.alt)
    {
        vib_terminal_write(VIB_NORMAL_BUFFER, sizeof(VIB_NORMAL_BUFFER) - 1);
    }
    else
    {
        vib_terminal_write(VIB_ALTERNATE_BUFFER, sizeof(VIB_ALTERNATE_BUFFER) - 1);
    }
    _terminal_state.alt = !_terminal_state.alt;
}

void vib_terminal_writef(BORROWED const char * fmt, ...)
{
    if (!fmt)
    {
        return;
    }
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    fflush(stdout);
    va_end(args);
}

void vib_terminal_writef_owned(OWNED char * fmt, ...)
{
    if (!fmt)
    {
        return;
    }
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    fflush(stdout);
    va_end(args);
    free_smart(fmt);
}

void vib_terminal_clear()
{
    vib_terminal_write(VIB_TERMINAL_CLR, sizeof(VIB_TERMINAL_CLR) - 1);
}

void vib_terminal_cursor_home()
{
    vib_terminal_write(VIB_CURSOR_HOME, sizeof(VIB_CURSOR_HOME) - 1);
}

void vib_terminal_cursor_move(COPIED uint64_t row, COPIED uint64_t column)
{
    char coord[25] = { 0 };
    int n = snprintf(coord, sizeof(coord), VIB_CURSOR_LOCATION, row, column);
    vib_terminal_write(coord, n);
}

void vib_terminal_cursor_hide()
{
    vib_terminal_write(VIB_CURSOR_HIDE, sizeof(VIB_CURSOR_HIDE) - 1);
}

void vib_terminal_cursor_show()
{
    vib_terminal_write(VIB_CURSOR_SHOW, sizeof(VIB_CURSOR_SHOW) - 1);
}


