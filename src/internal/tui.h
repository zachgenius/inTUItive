#pragma once

/**
 * Internal TUI functions
 */

/**
 * Set cursor position to be shown after rendering
 * Called by renderer when an input is focused
 */
void tui_set_cursor(int x, int y);
