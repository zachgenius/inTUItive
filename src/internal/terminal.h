#ifndef INTUITIVE_TERMINAL_H
#define INTUITIVE_TERMINAL_H

#include <stdbool.h>

/**
 * Terminal abstraction layer
 * Provides low-level terminal control using ANSI escape codes
 */

/**
 * Initialize terminal for TUI mode
 * - Enters raw mode (no buffering, no echo)
 * - Switches to alternate screen buffer
 * - Hides cursor
 * Returns true on success, false on failure
 */
bool term_init(void);

/**
 * Restore terminal to normal mode
 * - Exits raw mode
 * - Restores main screen buffer
 * - Shows cursor
 */
void term_cleanup(void);

/**
 * Clear the entire screen
 */
void term_clear(void);

/**
 * Move cursor to position (x, y)
 * Top-left is (0, 0)
 */
void term_move_cursor(int x, int y);

/**
 * Write string at current cursor position
 */
void term_write(const char* str);

/**
 * Get terminal dimensions
 * Stores width and height in provided pointers
 * Returns true on success, false on failure
 */
bool term_get_size(int* width, int* height);

/**
 * Hide the cursor
 */
void term_hide_cursor(void);

/**
 * Show the cursor
 */
void term_show_cursor(void);

#endif // INTUITIVE_TERMINAL_H
