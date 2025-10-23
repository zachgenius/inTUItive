#include "internal/terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

// ANSI escape codes
#define ANSI_CLEAR "\033[2J"
#define ANSI_HOME "\033[H"
#define ANSI_HIDE_CURSOR "\033[?25l"
#define ANSI_SHOW_CURSOR "\033[?25h"
#define ANSI_ALT_BUFFER "\033[?1049h"
#define ANSI_MAIN_BUFFER "\033[?1049l"

// Original terminal settings to restore on cleanup
static struct termios original_termios;
static bool termios_saved = false;

bool term_init(void) {
    // Save current terminal settings
    if (tcgetattr(STDIN_FILENO, &original_termios) == -1) {
        perror("tcgetattr");
        return false;
    }
    termios_saved = true;

    // Configure terminal for raw mode
    struct termios raw = original_termios;

    // Disable canonical mode (line buffering) and echo
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

    // Disable input processing
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);

    // Disable output processing
    raw.c_oflag &= ~(OPOST);

    // Set character size to 8 bits
    raw.c_cflag |= (CS8);

    // Read returns immediately with any data
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    // Apply settings
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        perror("tcsetattr");
        return false;
    }

    // Switch to alternate screen buffer
    term_write(ANSI_ALT_BUFFER);

    // Hide cursor
    term_hide_cursor();

    // Clear screen
    term_clear();

    return true;
}

void term_cleanup(void) {
    // Show cursor
    term_show_cursor();

    // Switch back to main screen buffer
    term_write(ANSI_MAIN_BUFFER);

    // Restore original terminal settings
    if (termios_saved) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
        termios_saved = false;
    }
}

void term_clear(void) {
    term_write(ANSI_CLEAR);
    term_write(ANSI_HOME);
}

void term_move_cursor(int x, int y) {
    // ANSI uses 1-based indexing, we use 0-based
    char buf[32];
    snprintf(buf, sizeof(buf), "\033[%d;%dH", y + 1, x + 1);
    term_write(buf);
}

void term_write(const char* str) {
    write(STDOUT_FILENO, str, strlen(str));
}

bool term_get_size(int* width, int* height) {
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        perror("ioctl");
        return false;
    }

    *width = ws.ws_col;
    *height = ws.ws_row;
    return true;
}

void term_hide_cursor(void) {
    term_write(ANSI_HIDE_CURSOR);
}

void term_show_cursor(void) {
    term_write(ANSI_SHOW_CURSOR);
}
