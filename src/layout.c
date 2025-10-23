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
            break;

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
