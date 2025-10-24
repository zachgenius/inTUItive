#include "internal/renderer.h"
#include "internal/component.h"
#include "internal/terminal.h"
#include "internal/tui.h"
#include <string.h>

void render_component(struct component_t* component) {
    if (!component) {
        return;
    }

    // Apply component styling
    bool has_style = (component->fg_color != COLOR_DEFAULT ||
                      component->bg_color != COLOR_DEFAULT ||
                      component->style != STYLE_NONE);

    if (has_style) {
        if (component->fg_color != COLOR_DEFAULT || component->bg_color != COLOR_DEFAULT) {
            term_set_color(component->fg_color, component->bg_color);
        }
        if (component->style != STYLE_NONE) {
            term_set_style(component->style);
        }
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

        case COMPONENT_LIST: {
            list_data_t* data = (list_data_t*)component->data;
            if (data && data->items) {
                int visible_count = component->height;
                int start_index = data->scroll_offset;
                int end_index = start_index + visible_count;

                if (end_index > data->item_count) {
                    end_index = data->item_count;
                }

                for (int i = start_index; i < end_index; i++) {
                    term_move_cursor(component->x, component->y + (i - start_index));
                    term_write(data->items[i]);
                }
            }
            break;
        }

        case COMPONENT_MODAL: {
            modal_data_t* data = (modal_data_t*)component->data;
            if (!data || !data->is_open || !*data->is_open) {
                break;
            }

            int x = component->x;
            int y = component->y;
            int w = component->width;
            int h = component->height;

            // Clear entire modal background first
            for (int row = 0; row < h; row++) {
                term_move_cursor(x, y + row);
                for (int col = 0; col < w; col++) {
                    term_write(" ");
                }
            }

            // Draw top border
            term_move_cursor(x, y);
            term_write("+");
            for (int col = 1; col < w - 1; col++) {
                term_write("-");
            }
            term_write("+");

            // Draw title if present
            if (data->title) {
                term_move_cursor(x + 2, y + 1);
                term_write(data->title);

                // Draw side borders for title row
                term_move_cursor(x, y + 1);
                term_write("|");
                term_move_cursor(x + w - 1, y + 1);
                term_write("|");
            }

            // Render content
            if (data->content) {
                render_component(data->content);
            }

            // Draw side borders for all content rows
            int start_row = data->title ? 2 : 1;
            for (int row = start_row; row < h - 1; row++) {
                term_move_cursor(x, y + row);
                term_write("|");
                term_move_cursor(x + w - 1, y + row);
                term_write("|");
            }

            // Draw bottom border
            term_move_cursor(x, y + h - 1);
            term_write("+");
            for (int col = 1; col < w - 1; col++) {
                term_write("-");
            }
            term_write("+");
            break;
        }

        case COMPONENT_VSTACK:
        case COMPONENT_HSTACK:
            for (int i = 0; i < component->child_count; i++) {
                render_component(component->children[i]);
            }
            break;
    }

    // Reset styling after rendering
    if (has_style) {
        term_reset_style();
    }
}
