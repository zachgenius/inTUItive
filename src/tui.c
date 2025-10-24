#include "../include/intuitive.h"
#include "internal/terminal.h"
#include "internal/component.h"
#include "internal/layout.h"
#include "internal/renderer.h"
#include "internal/events.h"
#include "internal/focus.h"
#include "internal/tui.h"
#include "internal/diff.h"
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
    component_t* prev_root;  // Previous tree for diffing
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

bool tui_get_terminal_size(int* width, int* height) {
    if (!width || !height) {
        return false;
    }

    if (term_get_size(&tui_state.term_width, &tui_state.term_height)) {
        *width = tui_state.term_width;
        *height = tui_state.term_height;
        return true;
    }

    return false;
}

void tui_set_cursor(int x, int y) {
    tui_state.show_cursor = true;
    tui_state.cursor_x = x;
    tui_state.cursor_y = y;
}

static struct component_t* find_open_modal(struct component_t* component) {
    if (!component) {
        return NULL;
    }

    if (component->type == COMPONENT_MODAL) {
        modal_data_t* data = (modal_data_t*)component->data;
        if (data && data->is_open && *data->is_open) {
            return component;
        }
    }

    for (int i = 0; i < component->child_count; i++) {
        struct component_t* modal = find_open_modal(component->children[i]);
        if (modal) {
            return modal;
        }
    }

    return NULL;
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
    } else if (focused->type == COMPONENT_SCROLLVIEW) {
        scrollview_data_t* data = (scrollview_data_t*)focused->data;
        if (!data || !data->content || !data->scroll_offset) {
            return false;
        }

        int key = event->data.key.code;
        int max_scroll = data->content->height - data->max_visible_height;
        if (max_scroll < 0) max_scroll = 0;

        if (key == KEY_UP) {
            if (*data->scroll_offset > 0) {
                (*data->scroll_offset)--;
                return true;
            }
        } else if (key == KEY_DOWN) {
            if (*data->scroll_offset < max_scroll) {
                (*data->scroll_offset)++;
                return true;
            }
        }
    } else if (focused->type == COMPONENT_LIST) {
        list_data_t* data = (list_data_t*)focused->data;
        if (!data || !data->selected_index) {
            return false;
        }

        int key = event->data.key.code;
        int selected = *data->selected_index;

        if (key == KEY_UP) {
            if (selected > 0) {
                (*data->selected_index)--;

                // Auto-scroll if selection moves off screen
                if (data->scroll_offset && *data->selected_index < *data->scroll_offset) {
                    *data->scroll_offset = *data->selected_index;
                }
                return true;
            }
        } else if (key == KEY_DOWN) {
            if (selected < data->item_count - 1) {
                (*data->selected_index)++;

                // Auto-scroll if selection moves off screen
                if (data->scroll_offset) {
                    int visible_end = *data->scroll_offset + data->max_visible_items;
                    if (*data->selected_index >= visible_end) {
                        *data->scroll_offset = *data->selected_index - data->max_visible_items + 1;
                    }
                }
                return true;
            }
        } else if (key == KEY_ENTER || key == '\r') {
            if (data->on_select) {
                data->on_select(*data->selected_index);
                return true;
            }
        }
    }

    return false;
}

/**
 * Find component at given coordinates (recursive)
 */
static struct component_t* find_component_at(struct component_t* component, int x, int y) {
    if (!component) {
        return NULL;
    }

    // Check children first (depth-first, so top-most components checked first)
    for (int i = component->child_count - 1; i >= 0; i--) {
        struct component_t* found = find_component_at(component->children[i], x, y);
        if (found) {
            return found;
        }
    }

    // Check if this component contains the point
    if (x >= component->x && x < component->x + component->width &&
        y >= component->y && y < component->y + component->height) {
        return component;
    }

    return NULL;
}

/**
 * Handle mouse click on a component
 */
static bool handle_mouse_click(struct component_t* component, int x, int y) {
    if (!component) {
        return false;
    }

    switch (component->type) {
        case COMPONENT_BUTTON: {
            button_data_t* data = (button_data_t*)component->data;
            if (data && data->on_click) {
                data->on_click();
                return true;
            }
            break;
        }

        case COMPONENT_LIST: {
            list_data_t* data = (list_data_t*)component->data;
            if (data && data->selected_index) {
                // Calculate which item was clicked
                int relative_y = y - component->y;
                int scroll_offset = data->scroll_offset ? *data->scroll_offset : 0;
                int clicked_index = scroll_offset + relative_y;

                if (clicked_index >= 0 && clicked_index < data->item_count) {
                    *data->selected_index = clicked_index;

                    // Call on_select if present
                    if (data->on_select) {
                        data->on_select(clicked_index);
                    }

                    return true;
                }
            }
            break;
        }

        default:
            break;
    }

    return false;
}

/**
 * Handle mouse scroll on a component
 */
static bool handle_mouse_scroll(struct component_t* component, int delta) {
    if (!component) {
        return false;
    }

    // Find scrollable component (List or ScrollView)
    if (component->type == COMPONENT_LIST) {
        list_data_t* data = (list_data_t*)component->data;
        if (data && data->scroll_offset) {
            int new_offset = *data->scroll_offset + delta;  // Positive delta = scroll down
            if (new_offset < 0) new_offset = 0;

            int max_offset = data->item_count - data->max_visible_items;
            if (max_offset < 0) max_offset = 0;
            if (new_offset > max_offset) new_offset = max_offset;

            if (new_offset != *data->scroll_offset) {
                *data->scroll_offset = new_offset;
                return true;
            }
        }
    } else if (component->type == COMPONENT_SCROLLVIEW) {
        scrollview_data_t* data = (scrollview_data_t*)component->data;
        if (data && data->scroll_offset && data->content) {
            int new_offset = *data->scroll_offset + delta;  // Positive delta = scroll down
            if (new_offset < 0) new_offset = 0;

            int max_offset = data->content->height - data->max_visible_height;
            if (max_offset < 0) max_offset = 0;
            if (new_offset > max_offset) new_offset = max_offset;

            if (new_offset != *data->scroll_offset) {
                *data->scroll_offset = new_offset;
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
            // Build new tree
            component_t* new_root = tui_state.root_fn();
            if (!new_root) {
                break;
            }

            // Measure and layout new tree
            layout_measure(new_root);
            layout_position(new_root, 0, 0);

            // Diff with previous tree
            bool has_changes = component_diff_trees(tui_state.prev_root, new_root);

            // Only render if there are actual changes
            if (has_changes || !tui_state.prev_root) {
                focus_build_list(new_root);
                term_clear();
                term_hide_cursor();

                tui_state.show_cursor = false;
                render_component(new_root);

                if (tui_state.show_cursor) {
                    term_move_cursor(tui_state.cursor_x, tui_state.cursor_y);
                    term_show_cursor();
                }
            }

            // Free old trees and update state
            if (tui_state.root) {
                component_free(tui_state.root);
            }
            if (tui_state.prev_root) {
                component_free(tui_state.prev_root);
            }

            tui_state.prev_root = tui_state.root;
            tui_state.root = new_root;
            tui_state.needs_render = false;
        }

        event_t event;
        if (event_poll(&event)) {
            if (event.type == EVENT_KEY) {
                int key = event.data.key.code;

                // Check if a modal is open
                struct component_t* open_modal = find_open_modal(tui_state.root);

                // Check for Esc to close modal (works always)
                if (key == KEY_ESC && open_modal) {
                    modal_data_t* data = (modal_data_t*)open_modal->data;
                    if (data && data->on_close) {
                        data->on_close();
                        tui_state.needs_render = true;
                    }
                } else if (key == KEY_TAB) {
                    focus_next();
                    tui_state.needs_render = true;
                } else {
                    struct component_t* focused = focus_get_current();
                    bool handled = handle_input_event(focused, &event);
                    if (handled) {
                        tui_state.needs_render = true;
                    } else {
                        // Key not consumed by component

                        // If modal is open with no focusable elements, any key closes it
                        if (open_modal && !focused) {
                            modal_data_t* data = (modal_data_t*)open_modal->data;
                            if (data && data->on_close) {
                                data->on_close();
                                tui_state.needs_render = true;
                            }
                        } else if (key == 'q' || key == 'Q') {
                            // Only allow quit if no modal is open
                            if (!open_modal) {
                                tui_state.running = false;
                            }
                        }
                    }
                }
            } else if (event.type == EVENT_MOUSE) {
                // Handle mouse events
                int mouse_x = event.data.mouse.x;
                int mouse_y = event.data.mouse.y;
                mouse_button_t button = event.data.mouse.button;
                mouse_action_t action = event.data.mouse.action;

                // Find component under mouse
                struct component_t* target = find_component_at(tui_state.root, mouse_x, mouse_y);

                // Focus follows mouse - if the component is focusable, focus it
                if (target && target->focusable && !target->focused) {
                    focus_set(target);
                    tui_state.needs_render = true;
                }

                // Handle left click
                if (button == MOUSE_LEFT && action == MOUSE_PRESS && target) {
                    bool handled = handle_mouse_click(target, mouse_x, mouse_y);
                    if (handled) {
                        tui_state.needs_render = true;
                    }
                }
                // Handle scroll wheel
                else if ((button == MOUSE_SCROLL_UP || button == MOUSE_SCROLL_DOWN) && action == MOUSE_PRESS && target) {
                    bool handled = handle_mouse_scroll(target, button == MOUSE_SCROLL_UP ? -1 : 1);
                    if (handled) {
                        tui_state.needs_render = true;
                    }
                }
            }
        }
    }

    if (tui_state.root) {
        component_free(tui_state.root);
    }
    if (tui_state.prev_root) {
        component_free(tui_state.prev_root);
    }
    term_cleanup();
}
