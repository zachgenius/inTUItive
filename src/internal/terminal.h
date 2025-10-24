#pragma once

#include <stdbool.h>
#include "../include/intuitive.h"

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

/**
 * Set foreground and background colors
 * Use COLOR_DEFAULT to keep current color
 */
void term_set_color(color_t fg, color_t bg);

/**
 * Set text style (bold, underline, etc.)
 */
void term_set_style(style_t style);

/**
 * Reset all colors and styles to default
 */
void term_reset_style(void);

/**
 * Enable mouse tracking
 * Terminal will report mouse events
 */
void term_enable_mouse(void);

/**
 * Disable mouse tracking
 */
void term_disable_mouse(void);
