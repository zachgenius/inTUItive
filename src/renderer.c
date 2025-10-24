#include "internal/renderer.h"
#include "internal/component.h"
#include "internal/terminal.h"
#include "internal/tui.h"
#include <string.h>
#include <stdbool.h>

// Clipping rectangle state
static bool clip_enabled = false;
static int clip_x, clip_y, clip_width, clip_height;

void render_set_clip(int x, int y, int width, int height) {
    clip_enabled = true;
    clip_x = x;
    clip_y = y;
    clip_width = width;
    clip_height = height;
}

void render_clear_clip(void) {
    clip_enabled = false;
}

bool render_is_clipped(int x, int y) {
    if (!clip_enabled) {
        return false;
    }
    return (x < clip_x || x >= clip_x + clip_width ||
            y < clip_y || y >= clip_y + clip_height);
}

// Helper function to pad string to fixed width
static void write_padded(const char* str, int width) {
    int len = strlen(str);
    if (len >= width) {
        // String too long, truncate
        for (int i = 0; i < width; i++) {
            char buf[2] = {str[i], '\0'};
            term_write(buf);
        }
    } else {
        // Write string and pad with spaces
        term_write(str);
        for (int i = len; i < width; i++) {
            term_write(" ");
        }
    }
}

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
                // Skip rendering if clipped
                if (render_is_clipped(component->x, component->y)) {
                    break;
                }
                term_move_cursor(component->x, component->y);
                term_write(data->content);
            }
            break;
        }

        case COMPONENT_BUTTON: {
            button_data_t* data = (button_data_t*)component->data;
            if (data && data->label) {
                // Skip rendering if clipped
                if (render_is_clipped(component->x, component->y)) {
                    break;
                }
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
                // Skip rendering if clipped
                if (render_is_clipped(component->x, component->y)) {
                    break;
                }

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
                int start_index = data->scroll_offset ? *data->scroll_offset : 0;
                int end_index = start_index + visible_count;

                if (end_index > data->item_count) {
                    end_index = data->item_count;
                }

                int selected = data->selected_index ? *data->selected_index : -1;

                for (int i = start_index; i < end_index; i++) {
                    int y = component->y + (i - start_index);
                    // Skip rendering if clipped
                    if (render_is_clipped(component->x, y)) {
                        continue;
                    }
                    term_move_cursor(component->x, y);

                    // Show selection indicator if this item is selected
                    if (i == selected && component->focused) {
                        term_write("> ");
                        term_write(data->items[i]);
                    } else if (i == selected) {
                        term_write("* ");
                        term_write(data->items[i]);
                    } else {
                        term_write("  ");
                        term_write(data->items[i]);
                    }
                }
            }
            break;
        }

        case COMPONENT_SCROLLVIEW: {
            scrollview_data_t* data = (scrollview_data_t*)component->data;
            if (data && data->content) {
                // Clear the viewport area first
                for (int row = 0; row < component->height; row++) {
                    term_move_cursor(component->x, component->y + row);
                    for (int col = 0; col < component->width; col++) {
                        term_write(" ");
                    }
                }

                // Show focus indicator if focused
                if (component->focused) {
                    term_move_cursor(component->x - 2, component->y);
                    term_write(">");
                    term_move_cursor(component->x + component->width, component->y);
                    term_write("<");
                }

                // Set clipping rectangle to viewport bounds
                render_set_clip(component->x, component->y, component->width, component->height);

                // Render the content - only content within clip rect will be rendered
                render_component(data->content);

                // Clear clipping
                render_clear_clip();

                // Show scroll indicators if enabled
                if (data->show_indicators) {
                    int scroll_offset = data->scroll_offset ? *data->scroll_offset : 0;
                    int content_height = data->content->height;
                    int viewport_height = component->height;

                    // Show up arrow if there's content above
                    if (scroll_offset > 0) {
                        term_move_cursor(component->x + component->width + 1, component->y);
                        term_write("▲");
                    }

                    // Show down arrow if there's content below
                    if (scroll_offset + viewport_height < content_height) {
                        term_move_cursor(component->x + component->width + 1, component->y + viewport_height - 1);
                        term_write("▼");
                    }
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

        case COMPONENT_TABLE: {
            table_data_t* data = (table_data_t*)component->data;
            if (!data) break;

            int x = component->x;
            int y = component->y;
            int current_y = y;

            if (data->show_borders) {
                // Top border
                term_move_cursor(x, current_y++);
                term_write("+");
                for (int col = 0; col < data->header_count; col++) {
                    for (int i = 0; i < data->column_widths[col] + 2; i++) {
                        term_write("-");
                    }
                    term_write("+");
                }

                // Header row
                term_move_cursor(x, current_y++);
                term_write("|");
                for (int col = 0; col < data->header_count; col++) {
                    term_write(" ");
                    write_padded(data->headers[col], data->column_widths[col]);
                    term_write(" |");
                }

                // Separator
                term_move_cursor(x, current_y++);
                term_write("+");
                for (int col = 0; col < data->header_count; col++) {
                    for (int i = 0; i < data->column_widths[col] + 2; i++) {
                        term_write("-");
                    }
                    term_write("+");
                }

                // Data rows
                for (int row = 0; row < data->row_count; row++) {
                    term_move_cursor(x, current_y++);
                    term_write("|");
                    for (int col = 0; col < data->header_count; col++) {
                        term_write(" ");
                        write_padded(data->rows[row][col], data->column_widths[col]);
                        term_write(" |");
                    }
                }
            } else {
                // Header row without borders
                term_move_cursor(x, current_y++);
                for (int col = 0; col < data->header_count; col++) {
                    write_padded(data->headers[col], data->column_widths[col]);
                    if (col < data->header_count - 1) {
                        term_write("  ");
                    }
                }

                // Separator line
                term_move_cursor(x, current_y++);
                for (int col = 0; col < data->header_count; col++) {
                    for (int i = 0; i < data->column_widths[col]; i++) {
                        term_write("-");
                    }
                    if (col < data->header_count - 1) {
                        term_write("  ");
                    }
                }

                // Data rows
                for (int row = 0; row < data->row_count; row++) {
                    term_move_cursor(x, current_y++);
                    for (int col = 0; col < data->header_count; col++) {
                        write_padded(data->rows[row][col], data->column_widths[col]);
                        if (col < data->header_count - 1) {
                            term_write("  ");
                        }
                    }
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

    // Reset styling after rendering
    if (has_style) {
        term_reset_style();
    }
}
