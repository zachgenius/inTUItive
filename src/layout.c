#include "internal/layout.h"
#include "internal/component.h"
#include <string.h>

/**
 * Measure a single component's size
 */
static void measure_component(struct component_t* component) {
    if (!component) {
        return;
    }

    switch (component->type) {
        case COMPONENT_TEXT: {
            text_data_t* data = (text_data_t*)component->data;
            component->width = data->content ? strlen(data->content) : 0;
            component->height = 1;
            break;
        }

        case COMPONENT_BUTTON: {
            button_data_t* data = (button_data_t*)component->data;
            component->width = data->label ? strlen(data->label) + 2 : 2;
            component->height = 1;
            break;
        }

        case COMPONENT_INPUT: {
            input_data_t* data = (input_data_t*)component->data;
            size_t content_len = strlen(data->buffer);
            size_t min_width = 20;
            size_t max_width = 60;

            if (content_len < min_width) {
                component->width = min_width + 2;
            } else if (content_len > max_width) {
                component->width = max_width + 2;
            } else {
                component->width = content_len + 2;
            }
            component->height = 1;
            break;
        }

        case COMPONENT_LIST: {
            list_data_t* data = (list_data_t*)component->data;
            int max_width = 0;

            for (int i = 0; i < data->item_count; i++) {
                int item_len = strlen(data->items[i]);
                if (item_len > max_width) {
                    max_width = item_len;
                }
            }

            component->width = max_width;
            int visible_items = data->item_count < data->max_visible_items ?
                                data->item_count : data->max_visible_items;
            component->height = visible_items;
            break;
        }

        case COMPONENT_MODAL: {
            modal_data_t* data = (modal_data_t*)component->data;
            if (data->content) {
                int title_len = data->title ? strlen(data->title) : 0;
                int content_width = data->content->width;
                int max_width = title_len > content_width ? title_len : content_width;

                component->width = max_width + 4;
                component->height = data->content->height + 4;
            } else {
                component->width = 20;
                component->height = 5;
            }
            break;
        }

        case COMPONENT_SCROLLVIEW: {
            scrollview_data_t* data = (scrollview_data_t*)component->data;
            if (data->content) {
                // ScrollView takes content's width but constrains height
                component->width = data->content->width;
                int content_height = data->content->height;
                component->height = content_height < data->max_visible_height ?
                                    content_height : data->max_visible_height;
            } else {
                component->width = 10;
                component->height = 10;
            }
            break;
        }

        case COMPONENT_TABLE: {
            table_data_t* data = (table_data_t*)component->data;
            // Calculate total width: sum of column widths + borders/padding
            int total_width = 0;
            for (int i = 0; i < data->header_count; i++) {
                total_width += data->column_widths[i];
            }

            if (data->show_borders) {
                // Add space for borders: "| col1 | col2 | col3 |"
                total_width += (data->header_count + 1) * 3; // "| " and " |" for each column
            } else {
                // Add space between columns
                total_width += (data->header_count - 1) * 2; // "  " between columns
            }

            component->width = total_width;

            // Height: header + separator + rows + borders
            if (data->show_borders) {
                component->height = 3 + data->row_count; // top border + header + sep + rows
            } else {
                component->height = 2 + data->row_count; // header + sep + rows
            }
            break;
        }

        case COMPONENT_SPINNER: {
            spinner_data_t* data = (spinner_data_t*)component->data;
            int width = 2;  // Spinner character (Unicode may be wider)

            if (data->text) {
                width += 1 + strlen(data->text);  // Space + text
            }

            if (data->progress) {
                width += 8;  // Space + "XXX.X%" (e.g., " 100.0%")
            }

            component->width = width;
            component->height = 1;
            break;
        }

        case COMPONENT_VSTACK: {
            // VStack: width = max(children), height = sum(children)
            int max_width = 0;
            int total_height = 0;

            for (int i = 0; i < component->child_count; i++) {
                struct component_t* child = component->children[i];
                if (child->width > max_width) {
                    max_width = child->width;
                }
                total_height += child->height;
            }

            // Add spacing between children if configured
            stack_data_t* data = (stack_data_t*)component->data;
            if (data && data->spacing > 0 && component->child_count > 1) {
                total_height += data->spacing * (component->child_count - 1);
            }

            component->width = max_width;
            component->height = total_height;
            break;
        }

        case COMPONENT_HSTACK: {
            // HStack: width = sum(children), height = max(children)
            int total_width = 0;
            int max_height = 0;

            for (int i = 0; i < component->child_count; i++) {
                struct component_t* child = component->children[i];
                total_width += child->width;
                if (child->height > max_height) {
                    max_height = child->height;
                }
            }

            // Add spacing between children if configured
            stack_data_t* data = (stack_data_t*)component->data;
            if (data && data->spacing > 0 && component->child_count > 1) {
                total_width += data->spacing * (component->child_count - 1);
            }

            component->width = total_width;
            component->height = max_height;
            break;
        }

        case COMPONENT_PADDING: {
            padding_data_t* data = (padding_data_t*)component->data;
            if (data && data->child) {
                // Padding adds to child's dimensions
                component->width = data->child->width + data->padding.left + data->padding.right;
                component->height = data->child->height + data->padding.top + data->padding.bottom;
            } else {
                component->width = 0;
                component->height = 0;
            }
            break;
        }

        case COMPONENT_SPACER: {
            // Spacer has flexible size - measure phase gives it 0 size
            // Layout phase will expand it to fill available space
            component->width = 0;
            component->height = 0;
            break;
        }
    }
}

void layout_measure(struct component_t* component) {
    if (!component) {
        return;
    }

    // Measure children first (bottom-up)
    for (int i = 0; i < component->child_count; i++) {
        layout_measure(component->children[i]);
    }

    // For modals, scrollviews, and padding, also measure the content/child
    if (component->type == COMPONENT_MODAL) {
        modal_data_t* data = (modal_data_t*)component->data;
        if (data && data->content) {
            layout_measure(data->content);
        }
    } else if (component->type == COMPONENT_SCROLLVIEW) {
        scrollview_data_t* data = (scrollview_data_t*)component->data;
        if (data && data->content) {
            layout_measure(data->content);
        }
    } else if (component->type == COMPONENT_PADDING) {
        padding_data_t* data = (padding_data_t*)component->data;
        if (data && data->child) {
            layout_measure(data->child);
        }
    }

    // Then measure this component
    measure_component(component);
}

void layout_position(struct component_t* component, int x, int y) {
    if (!component) {
        return;
    }

    // Set this component's position
    component->x = x;
    component->y = y;

    switch (component->type) {
        case COMPONENT_TEXT:
        case COMPONENT_BUTTON:
        case COMPONENT_INPUT:
        case COMPONENT_LIST:
        case COMPONENT_TABLE:
        case COMPONENT_SPACER:
        case COMPONENT_SPINNER:
            break;

        case COMPONENT_MODAL: {
            modal_data_t* data = (modal_data_t*)component->data;
            if (data && data->content) {
                // Center modal on screen (ignore x, y from layout)
                int modal_x = 10;
                int modal_y = 5;

                component->x = modal_x;
                component->y = modal_y;

                int content_x = modal_x + 2;
                int content_y = modal_y + 2;
                if (data->title) {
                    content_y++;
                }
                layout_position(data->content, content_x, content_y);
            }
            break;
        }

        case COMPONENT_SCROLLVIEW: {
            scrollview_data_t* data = (scrollview_data_t*)component->data;
            if (data && data->content) {
                // Position content, offset by visual (animated) scroll amount
                int visual_offset = (int)(data->visual_scroll_offset + 0.5f);
                layout_position(data->content, x, y - visual_offset);
            }
            break;
        }

        case COMPONENT_PADDING: {
            padding_data_t* data = (padding_data_t*)component->data;
            if (data && data->child) {
                // Position child inside padding
                int child_x = x + data->padding.left;
                int child_y = y + data->padding.top;
                layout_position(data->child, child_x, child_y);
            }
            break;
        }

        case COMPONENT_VSTACK: {
            stack_data_t* data = (stack_data_t*)component->data;
            int spacing = data ? data->spacing : 0;
            alignment_t alignment = data ? data->alignment : ALIGN_START;

            // Stack children vertically
            int current_y = y;
            for (int i = 0; i < component->child_count; i++) {
                struct component_t* child = component->children[i];

                // Handle horizontal alignment
                int child_x = x;
                if (alignment == ALIGN_CENTER) {
                    child_x = x + (component->width - child->width) / 2;
                } else if (alignment == ALIGN_END) {
                    child_x = x + (component->width - child->width);
                }

                layout_position(child, child_x, current_y);
                current_y += child->height + spacing;
            }
            break;
        }

        case COMPONENT_HSTACK: {
            stack_data_t* data = (stack_data_t*)component->data;
            int spacing = data ? data->spacing : 0;
            alignment_t alignment = data ? data->alignment : ALIGN_START;

            // Stack children horizontally
            int current_x = x;
            for (int i = 0; i < component->child_count; i++) {
                struct component_t* child = component->children[i];

                // Handle vertical alignment
                int child_y = y;
                if (alignment == ALIGN_CENTER) {
                    child_y = y + (component->height - child->height) / 2;
                } else if (alignment == ALIGN_END) {
                    child_y = y + (component->height - child->height);
                }

                layout_position(child, current_x, child_y);
                current_x += child->width + spacing;
            }
            break;
        }
    }
}
