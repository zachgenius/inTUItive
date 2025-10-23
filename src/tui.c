#include "../include/intuitive.h"
#include "internal/terminal.h"
#include "internal/component.h"
#include "internal/layout.h"
#include "internal/renderer.h"
#include "internal/events.h"
#include "internal/focus.h"
#include "internal/tui.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

/**
 * TUI state
 */
typedef struct {
    component_t* (*root_fn)(void);
    component_t* root;
    bool running;
    bool needs_render;
    int term_width;
    int term_height;
    bool show_cursor;
    int cursor_x;
    int cursor_y;
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

void tui_request_render(void) {
    tui_state.needs_render = true;
}

void tui_set_cursor(int x, int y) {
    tui_state.show_cursor = true;
    tui_state.cursor_x = x;
    tui_state.cursor_y = y;
}

static bool handle_input_event(struct component_t* focused, event_t* event) {
    if (!focused || event->type != EVENT_KEY) {
        return false;
    }

    if (focused->type == COMPONENT_INPUT) {
        input_data_t* data = (input_data_t*)focused->data;
        int key = event->data.key.code;

        if (isprint(key)) {
            size_t len = strlen(data->buffer);
            if (len + 1 < data->buffer_size) {
                memmove(&data->buffer[data->cursor_pos + 1],
                       &data->buffer[data->cursor_pos],
                       len - data->cursor_pos + 1);
                data->buffer[data->cursor_pos] = (char)key;
                data->cursor_pos++;
                return true;
            }
        } else if (key == KEY_BACKSPACE || key == 8) {
            if (data->cursor_pos > 0) {
                size_t len = strlen(data->buffer);
                memmove(&data->buffer[data->cursor_pos - 1],
                       &data->buffer[data->cursor_pos],
                       len - data->cursor_pos + 1);
                data->cursor_pos--;
                return true;
            }
        } else if (key == KEY_LEFT) {
            if (data->cursor_pos > 0) {
                data->cursor_pos--;
                return true;
            }
        } else if (key == KEY_RIGHT) {
            if (data->cursor_pos < strlen(data->buffer)) {
                data->cursor_pos++;
                return true;
            }
        }
    } else if (focused->type == COMPONENT_BUTTON) {
        int key = event->data.key.code;
        if (key == KEY_ENTER || key == '\r' || key == ' ') {
            button_data_t* data = (button_data_t*)focused->data;
            if (data && data->on_click) {
                data->on_click();
                return true;
            }
        }
    }

    return false;
}

void tui_run(void) {
    if (!tui_state.root_fn) {
        return;
    }

    tui_state.running = true;
    tui_state.needs_render = true;

    while (tui_state.running) {
        if (tui_state.needs_render) {
            if (tui_state.root) {
                component_free(tui_state.root);
            }

            tui_state.root = tui_state.root_fn();
            if (!tui_state.root) {
                break;
            }

            focus_build_list(tui_state.root);
            layout_measure(tui_state.root);
            layout_position(tui_state.root, 0, 0);
            term_clear();
            term_hide_cursor();

            tui_state.show_cursor = false;
            render_component(tui_state.root);

            if (tui_state.show_cursor) {
                term_move_cursor(tui_state.cursor_x, tui_state.cursor_y);
                term_show_cursor();
            }

            tui_state.needs_render = false;
        }

        event_t event;
        if (event_poll(&event)) {
            if (event.type == EVENT_KEY) {
                if (event.data.key.code == KEY_TAB) {
                    focus_next();
                    tui_state.needs_render = true;
                } else {
                    struct component_t* focused = focus_get_current();
                    bool handled = handle_input_event(focused, &event);
                    if (handled) {
                        tui_state.needs_render = true;
                    } else {
                        // Key not consumed by component - check for quit
                        int key = event.data.key.code;
                        if (key == 'q' || key == 'Q') {
                            tui_state.running = false;
                        }
                    }
                }
            }
        }
    }

    if (tui_state.root) {
        component_free(tui_state.root);
    }
    term_cleanup();
}
