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

            component->width = total_width;
            component->height = max_height;
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

    // For modals and scrollviews, also measure the content
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
            if (data && data->content && data->scroll_offset) {
                // Position content, offset by scroll amount
                layout_position(data->content, x, y - *data->scroll_offset);
            }
            break;
        }

        case COMPONENT_VSTACK: {
            // Stack children vertically
            int current_y = y;
            for (int i = 0; i < component->child_count; i++) {
                struct component_t* child = component->children[i];
                layout_position(child, x, current_y);
                current_y += child->height;
            }
            break;
        }

        case COMPONENT_HSTACK: {
            // Stack children horizontally
            int current_x = x;
            for (int i = 0; i < component->child_count; i++) {
                struct component_t* child = component->children[i];
                layout_position(child, current_x, y);
                current_x += child->width;
            }
            break;
        }
    }
}
