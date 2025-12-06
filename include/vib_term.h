#pragma once

/*
 * vib_term — Terminal control module
 *
 * Handles raw mode, screen operations, terminal size,
 * and ensures clean restoration on exit.
 */
#include <unistd.h>
#include <stdarg.h>

#include "common.h"
#include "result.h"

/**
 * Initialize terminal for TUI operation.
 * - Enters raw mode (no echo, no canonical, no signals)
 * - Registers atexit and signal handlers
 * - Queries initial terminal size
 *
 */
COPIED result_t vib_terminal_init();

/**
 * Restore terminal to original state.
 * Safe to call multiple times.
 */
void vib_terminal_quit();

/* ─────────────────────────────────────────────────────────────────────────────
 * Terminal Info
 * ───────────────────────────────────────────────────────────────────────────── */

COPIED uint64_t vib_terminal_get_rows();
COPIED uint64_t vib_terminal_get_columns();
COPIED bool vib_terminal_was_resized(void);
void vib_terminal_size_update();


/* ─────────────────────────────────────────────────────────────────────────────
 * Terminal Screen (TUI) Operations
 * ───────────────────────────────────────────────────────────────────────────── */

#define vib_terminal_write(sequence, len)       \
        write(STDOUT_FILENO, sequence, len)

void vib_terminal_writef(BORROWED const char * fmt, ...);
void vib_terminal_writef_owned(OWNED char * fmt, ...);

/** Clear entire screen. */
void vib_terminal_clear();

/** Move cursor to top-left (1,1). */
void vib_terminal_cursor_home();

/** Move cursor to specified position (1-indexed). */
void vib_terminal_cursor_move(COPIED uint64_t row, COPIED uint64_t column);

/** Hide cursor. */
void vib_terminal_cursor_hide();

/** Show cursor. */
void vib_terminal_cursor_show();

void vib_tui_use_normal_buffer();
void vib_tui_use_alternate_buffer();
void vib_tui_toggle_buffer();
void vib_terminal_flush();

