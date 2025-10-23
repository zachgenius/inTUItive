#include "internal/renderer.h"
#include "internal/component.h"
#include "internal/terminal.h"
#include "internal/tui.h"
#include <string.h>

void render_component(struct component_t* component) {
    if (!component) {
        return;
    }

    switch (component->type) {
        case COMPONENT_TEXT: {
            text_data_t* data = (text_data_t*)component->data;
            if (data && data->content) {
                term_move_cursor(component->x, component->y);
                term_write(data->content);
            }
            break;
        }

        case COMPONENT_BUTTON: {
            button_data_t* data = (button_data_t*)component->data;
            if (data && data->label) {
                term_move_cursor(component->x, component->y);
                if (component->focused) {
                    term_write(">");
                    term_write(data->label);
                    term_write("<");
                } else {
                    term_write("[");
                    term_write(data->label);
                    term_write("]");
                }
            }
            break;
        }

        case COMPONENT_INPUT: {
            input_data_t* data = (input_data_t*)component->data;
            if (data && data->buffer) {
                int display_width = component->width - 2;
                size_t content_len = strlen(data->buffer);

                size_t scroll = data->scroll_offset;
                if (data->cursor_pos < scroll) {
                    scroll = data->cursor_pos;
                } else if (data->cursor_pos >= scroll + display_width) {
                    scroll = data->cursor_pos - display_width + 1;
                }
                data->scroll_offset = scroll;

                term_move_cursor(component->x, component->y);
                term_write("[");

                size_t visible_start = scroll;
                size_t visible_end = scroll + display_width;
                if (visible_end > content_len) {
                    visible_end = content_len;
                }

                for (size_t i = visible_start; i < visible_end; i++) {
                    char buf[2] = {data->buffer[i], '\0'};
                    term_write(buf);
                }

                for (size_t i = visible_end - visible_start; i < (size_t)display_width; i++) {
                    term_write(" ");
                }

                term_write("]");

                if (component->focused) {
                    tui_set_cursor(component->x + 1 + (data->cursor_pos - scroll), component->y);
                }
            }
            break;
        }

        case COMPONENT_VSTACK:
        case COMPONENT_HSTACK:
            for (int i = 0; i < component->child_count; i++) {
                render_component(component->children[i]);
            }
            break;
    }
}
