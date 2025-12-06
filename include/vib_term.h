#pragma once

/*
 * vib_term — Terminal control module
 *
 * Handles raw mode, screen operations, terminal size,
 * and ensures clean restoration on exit.
 */

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

COPIED uint64_t vib_terminal_get_rows();
COPIED uint64_t vib_terminal_get_columns();

/**
 * Refresh terminal size (call after resize).
 */
void vib_terminal_size_update();

/**
 * Check if terminal was resized since last check.
 * Clears the flag after reading.
 */
COPIED bool vib_terminal_was_resized(void);

