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
COPIED result_t vib_term_init();

/**
 * Restore terminal to original state.
 * Safe to call multiple times.
 */
void vib_term_quit();
