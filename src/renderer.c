#include "internal/renderer.h"
#include "internal/component.h"
#include "internal/terminal.h"
#include "internal/tui.h"
#include "internal/animation.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

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
                // Get external target scroll offset
                int external_offset = data->scroll_offset ? *data->scroll_offset : 0;

                // Check if target changed - if so, start smooth scroll animation
                if (external_offset != data->target_scroll_offset) {
                    // Clean up old animation if exists
                    if (data->scroll_animation) {
                        anim_free(data->scroll_animation);
                    }

                    // Create smooth scroll animation (150ms, ease-out)
                    data->scroll_animation = anim_create(
                        data->visual_scroll_offset,
                        (float)external_offset,
                        150,  // 150ms duration
                        EASE_OUT
                    );
                    anim_start(data->scroll_animation);
                    data->target_scroll_offset = external_offset;
                }

                // Update animation if active
                if (data->scroll_animation) {
                    if (anim_update(data->scroll_animation)) {
                        data->visual_scroll_offset = anim_get_value(data->scroll_animation);
                        tui_request_render();  // Continue animating
                    } else {
                        // Animation complete
                        data->visual_scroll_offset = (float)data->target_scroll_offset;
                        anim_free(data->scroll_animation);
                        data->scroll_animation = NULL;
                    }
                }

                // Use visual (animated) scroll offset for rendering
                int visible_count = component->height;
                int start_index = (int)(data->visual_scroll_offset + 0.5f);  // Round to nearest
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
                // Get external target scroll offset
                int external_offset = data->scroll_offset ? *data->scroll_offset : 0;

                // Check if target changed - if so, start smooth scroll animation
                if (external_offset != data->target_scroll_offset) {
                    // Clean up old animation if exists
                    if (data->scroll_animation) {
                        anim_free(data->scroll_animation);
                    }

                    // Create smooth scroll animation (150ms, ease-out)
                    data->scroll_animation = anim_create(
                        data->visual_scroll_offset,
                        (float)external_offset,
                        150,  // 150ms duration
                        EASE_OUT
                    );
                    anim_start(data->scroll_animation);
                    data->target_scroll_offset = external_offset;
                }

                // Update animation if active
                if (data->scroll_animation) {
                    if (anim_update(data->scroll_animation)) {
                        data->visual_scroll_offset = anim_get_value(data->scroll_animation);
                        tui_request_render();  // Continue animating
                    } else {
                        // Animation complete
                        data->visual_scroll_offset = (float)data->target_scroll_offset;
                        anim_free(data->scroll_animation);
                        data->scroll_animation = NULL;
                    }
                }

                // Use visual (animated) scroll offset
                int scroll_offset = (int)(data->visual_scroll_offset + 0.5f);  // Round to nearest

                // Clear the viewport area first
                for (int row = 0; row < component->height; row++) {
                    term_move_cursor(component->x, component->y + row);
                    for (int col = 0; col < component->width; col++) {
                        term_write(" ");
                    }
                }

                // Set clipping rectangle to viewport bounds
                render_set_clip(component->x, component->y, component->width, component->height);

                // Render the content - only content within clip rect will be rendered
                render_component(data->content);

                // Clear clipping
                render_clear_clip();

                // Draw scroll bar if there's scrollable content
                if (data->show_indicators) {
                    int content_height = data->content->height;
                    int viewport_height = component->height;

                    if (content_height > viewport_height) {
                        int scrollbar_x = component->x + component->width + 1;

                        // Calculate thumb position and size
                        // thumb_size = (viewport / content) * viewport
                        int thumb_size = (viewport_height * viewport_height) / content_height;
                        if (thumb_size < 1) thumb_size = 1;
                        if (thumb_size > viewport_height) thumb_size = viewport_height;

                        // thumb_pos = (scroll / max_scroll) * (viewport - thumb_size)
                        int max_scroll = content_height - viewport_height;
                        int thumb_travel = viewport_height - thumb_size;
                        int thumb_pos = 0;
                        if (max_scroll > 0) {
                            thumb_pos = (scroll_offset * thumb_travel) / max_scroll;
                        }

                        // Draw the scroll bar
                        for (int row = 0; row < viewport_height; row++) {
                            term_move_cursor(scrollbar_x, component->y + row);

                            if (row >= thumb_pos && row < thumb_pos + thumb_size) {
                                // Draw thumb (use configured characters)
                                if (component->focused) {
                                    term_write(data->thumb_focused);
                                } else {
                                    term_write(data->thumb_unfocused);
                                }
                            } else {
                                // Draw track (use configured character)
                                term_write(data->track_char);
                            }
                        }

                        // Draw arrows at top and bottom if configured and there's content in that direction
                        if (data->show_arrows) {
                            if (scroll_offset > 0) {
                                term_move_cursor(scrollbar_x, component->y);
                                term_write("▲");
                            }
                            if (scroll_offset + viewport_height < content_height) {
                                term_move_cursor(scrollbar_x, component->y + viewport_height - 1);
                                term_write("▼");
                            }
                        }
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

        case COMPONENT_SPINNER: {
            spinner_data_t* data = (spinner_data_t*)component->data;
            if (!data) break;

            // Skip rendering if clipped
            if (render_is_clipped(component->x, component->y)) {
                break;
            }

            // Update animation frame if enough time has passed
            uint64_t now = anim_get_time_us();
            uint64_t elapsed_ms = (now - data->last_update_time_us) / 1000;

            if (elapsed_ms >= (uint64_t)data->speed_ms) {
                // Determine frame count for this style
                int frame_count;
                const char** frames;
                switch (data->style) {
                    case SPINNER_BRAILLE:
                        frame_count = 10;
                        break;
                    case SPINNER_CLASSIC:
                        frame_count = 4;
                        break;
                    case SPINNER_DOTS:
                    case SPINNER_ARROW:
                        frame_count = 8;
                        break;
                    case SPINNER_BOX:
                        frame_count = 4;
                        break;
                    default:
                        frame_count = 10;
                        break;
                }

                data->frame_index = (data->frame_index + 1) % frame_count;
                data->last_update_time_us = now;

                // Request re-render for next frame
                tui_request_render();
            }

            // Render current frame
            term_move_cursor(component->x, component->y);

            // Get the appropriate frame
            const char* frame = NULL;
            switch (data->style) {
                case SPINNER_BRAILLE: {
                    const char* frames_braille[] = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
                    frame = frames_braille[data->frame_index];
                    break;
                }
                case SPINNER_CLASSIC: {
                    const char* frames_classic[] = {"|", "/", "-", "\\"};
                    frame = frames_classic[data->frame_index];
                    break;
                }
                case SPINNER_DOTS: {
                    const char* frames_dots[] = {"⣾", "⣽", "⣻", "⢿", "⡿", "⣟", "⣯", "⣷"};
                    frame = frames_dots[data->frame_index];
                    break;
                }
                case SPINNER_BOX: {
                    const char* frames_box[] = {"◰", "◳", "◲", "◱"};
                    frame = frames_box[data->frame_index];
                    break;
                }
                case SPINNER_ARROW: {
                    const char* frames_arrow[] = {"←", "↖", "↑", "↗", "→", "↘", "↓", "↙"};
                    frame = frames_arrow[data->frame_index];
                    break;
                }
            }

            if (frame) {
                term_write(frame);

                // Render text if present
                if (data->text) {
                    term_write(" ");
                    term_write(data->text);
                }

                // Render progress if present
                if (data->progress) {
                    char progress_str[32];
                    snprintf(progress_str, sizeof(progress_str), " %.1f%%", *data->progress);
                    term_write(progress_str);
                }
            }
            break;
        }

        case COMPONENT_TOAST: {
            toast_data_t* data = (toast_data_t*)component->data;
            if (!data || !data->message) {
                break;
            }

            // Check if toast should be visible
            if (!data->is_visible || !*data->is_visible) {
                break;
            }

            uint64_t now = anim_get_time_us();
            int term_width, term_height;
            tui_get_terminal_size(&term_width, &term_height);

            // Calculate toast dimensions
            int msg_len = strlen(data->message);
            int toast_width = msg_len + 4;  // +4 for borders and padding
            int toast_height = 3;  // Top border, message, bottom border

            // Calculate base position based on position type
            int base_x, base_y;
            int slide_distance = toast_width + 2;  // Distance to slide from off-screen

            switch (data->position) {
                case TOAST_TOP:
                    base_x = (term_width - toast_width) / 2;
                    base_y = 1;
                    slide_distance = toast_height + 1;
                    break;
                case TOAST_BOTTOM:
                    base_x = (term_width - toast_width) / 2;
                    base_y = term_height - toast_height - 1;
                    slide_distance = toast_height + 1;
                    break;
                case TOAST_TOP_RIGHT:
                    base_x = term_width - toast_width - 2;
                    base_y = 1;
                    break;
                case TOAST_BOTTOM_RIGHT:
                    base_x = term_width - toast_width - 2;
                    base_y = term_height - toast_height - 1;
                    break;
            }

            // State machine for toast animation
            float offset = 0.0f;

            switch (data->state) {
                case TOAST_STATE_SLIDING_IN:
                    // Start slide-in animation if not already started
                    if (!data->slide_animation) {
                        data->slide_animation = anim_create(
                            (float)slide_distance,  // From off-screen
                            0.0f,                   // To on-screen
                            300,                    // 300ms duration
                            EASE_OUT
                        );
                        anim_start(data->slide_animation);
                    }

                    // Update animation
                    if (anim_update(data->slide_animation)) {
                        offset = anim_get_value(data->slide_animation);
                        tui_request_render();  // Continue animating
                    } else {
                        // Animation complete, transition to visible state
                        data->state = TOAST_STATE_VISIBLE;
                        data->visible_start_time = now;
                        anim_free(data->slide_animation);
                        data->slide_animation = NULL;
                        offset = 0.0f;
                    }
                    break;

                case TOAST_STATE_VISIBLE:
                    offset = 0.0f;

                    // Check if auto-dismiss timer expired
                    if (data->duration_ms > 0) {
                        uint64_t elapsed_ms = (now - data->visible_start_time) / 1000;
                        if (elapsed_ms >= (uint64_t)data->duration_ms) {
                            // Start slide-out animation
                            data->state = TOAST_STATE_SLIDING_OUT;
                        }
                    }
                    break;

                case TOAST_STATE_SLIDING_OUT:
                    // Start slide-out animation if not already started
                    if (!data->slide_animation) {
                        data->slide_animation = anim_create(
                            0.0f,                   // From on-screen
                            (float)slide_distance,  // To off-screen
                            200,                    // 200ms duration
                            EASE_IN
                        );
                        anim_start(data->slide_animation);
                    }

                    // Update animation
                    if (anim_update(data->slide_animation)) {
                        offset = anim_get_value(data->slide_animation);
                        tui_request_render();  // Continue animating
                    } else {
                        // Animation complete, hide toast
                        data->state = TOAST_STATE_HIDDEN;
                        *data->is_visible = false;
                        anim_free(data->slide_animation);
                        data->slide_animation = NULL;

                        // Call on_close callback if provided
                        if (data->on_close) {
                            data->on_close();
                        }
                    }
                    break;

                case TOAST_STATE_HIDDEN:
                    // Don't render
                    break;
            }

            // Calculate final position with animation offset
            int final_x = base_x;
            int final_y = base_y;

            if (data->position == TOAST_TOP || data->position == TOAST_BOTTOM) {
                // Slide vertically
                final_y = (data->position == TOAST_TOP) ?
                          (base_y - (int)offset) :
                          (base_y + (int)offset);
            } else {
                // Slide horizontally (from right)
                final_x = base_x + (int)offset;
            }

            // Only render if on-screen
            if (data->state != TOAST_STATE_HIDDEN &&
                final_x >= 0 && final_x < term_width &&
                final_y >= 0 && final_y < term_height) {

                // Draw toast background and border
                for (int row = 0; row < toast_height; row++) {
                    int y = final_y + row;
                    if (y < 0 || y >= term_height) continue;

                    term_move_cursor(final_x, y);

                    if (row == 0) {
                        // Top border
                        term_write("+");
                        for (int i = 0; i < toast_width - 2; i++) {
                            term_write("-");
                        }
                        term_write("+");
                    } else if (row == toast_height - 1) {
                        // Bottom border
                        term_write("+");
                        for (int i = 0; i < toast_width - 2; i++) {
                            term_write("-");
                        }
                        term_write("+");
                    } else {
                        // Message row
                        term_write("| ");
                        term_write(data->message);
                        term_write(" |");
                    }
                }
            }
            break;
        }

        case COMPONENT_PADDING: {
            padding_data_t* data = (padding_data_t*)component->data;
            if (data && data->child) {
                // Just render the child - layout already positioned it with padding offset
                render_component(data->child);
            }
            break;
        }

        case COMPONENT_SPACER:
            // Spacer renders nothing - it only affects layout
            break;

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
