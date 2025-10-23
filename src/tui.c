#include "../include/intuitive.h"
#include "internal/terminal.h"
#include "internal/component.h"
#include "internal/layout.h"
#include "internal/renderer.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

/**
 * TUI state
 */
typedef struct {
    component_t* (*root_fn)(void);  // Root component function
    component_t* root;               // Current root component tree
    bool running;                    // Event loop flag
    int term_width;
    int term_height;
} tui_state_t;

static tui_state_t tui_state = {0};

void tui_init(void) {
    // Initialize terminal
    if (!term_init()) {
        exit(1);
    }

    // Get terminal size
    term_get_size(&tui_state.term_width, &tui_state.term_height);
}

void tui_set_root(component_t* (*root_fn)(void)) {
    tui_state.root_fn = root_fn;
}

void tui_run(void) {
    if (!tui_state.root_fn) {
        return;
    }

    tui_state.running = true;

    // Build component tree
    tui_state.root = tui_state.root_fn();
    if (!tui_state.root) {
        term_cleanup();
        return;
    }

    // Layout pass
    layout_measure(tui_state.root);
    layout_position(tui_state.root, 0, 0);

    // Clear and render
    term_clear();
    render_component(tui_state.root);

    // Simple event loop - wait for 'q' to quit
    while (tui_state.running) {
        char c;
        if (read(STDIN_FILENO, &c, 1) == 1) {
            if (c == 'q' || c == 'Q') {
                tui_state.running = false;
            }
        }
    }

    // Cleanup
    component_free(tui_state.root);
    term_cleanup();
}
