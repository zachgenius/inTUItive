#include "internal/component.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct component_t* component_create(component_type_t type) {
    struct component_t* component = calloc(1, sizeof(struct component_t));
    if (!component) {
        return NULL;
    }

    component->type = type;
    component->data = NULL;
    component->children = NULL;
    component->child_count = 0;
    component->child_capacity = 0;
    component->x = 0;
    component->y = 0;
    component->width = 0;
    component->height = 0;
    component->focusable = false;
    component->focused = false;
    component->focus_index = -1;

    return component;
}

bool component_add_child(struct component_t* parent, struct component_t* child) {
    if (!parent || !child) {
        return false;
    }

    // Grow children array if needed
    if (parent->child_count >= parent->child_capacity) {
        int new_capacity = parent->child_capacity == 0 ? 4 : parent->child_capacity * 2;
        struct component_t** new_children = realloc(parent->children,
                                             new_capacity * sizeof(struct component_t*));
        if (!new_children) {
            return false;
        }
        parent->children = new_children;
        parent->child_capacity = new_capacity;
    }

    parent->children[parent->child_count++] = child;
    return true;
}

void component_free(struct component_t* component) {
    if (!component) {
        return;
    }

    // Free component-specific data
    if (component->data) {
        switch (component->type) {
            case COMPONENT_TEXT: {
                text_data_t* text_data = (text_data_t*)component->data;
                free(text_data->content);
                free(text_data);
                break;
            }
            case COMPONENT_BUTTON: {
                button_data_t* button_data = (button_data_t*)component->data;
                free(button_data->label);
                free(button_data);
                break;
            }
            case COMPONENT_INPUT: {
                input_data_t* input_data = (input_data_t*)component->data;
                free(input_data);
                break;
            }
            case COMPONENT_VSTACK:
            case COMPONENT_HSTACK:
                break;
        }
    }

    // Recursively free all children
    for (int i = 0; i < component->child_count; i++) {
        component_free(component->children[i]);
    }
    free(component->children);

    // Free the component itself
    free(component);
}

void component_set_data(struct component_t* component, void* data) {
    if (component) {
        component->data = data;
    }
}
