#include "internal/focus.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_FOCUSABLE 64

static struct component_t* focusable_list[MAX_FOCUSABLE];
static int focusable_count = 0;
static int current_focus_index = -1;

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

static void build_focusable_list_recursive(struct component_t* component) {
    if (!component) {
        return;
    }

    if (component->focusable && focusable_count < MAX_FOCUSABLE) {
        component->focus_index = focusable_count;
        focusable_list[focusable_count++] = component;
    }

    // For modals, only traverse the content, not other children
    if (component->type == COMPONENT_MODAL) {
        modal_data_t* data = (modal_data_t*)component->data;
        if (data && data->is_open && *data->is_open && data->content) {
            build_focusable_list_recursive(data->content);
        }
        return;
    }

    for (int i = 0; i < component->child_count; i++) {
        build_focusable_list_recursive(component->children[i]);
    }
}

void focus_build_list(struct component_t* root) {
    int prev_focus = current_focus_index;
    focus_clear();

    // Check if there's an open modal - if so, only collect focus from it
    struct component_t* open_modal = find_open_modal(root);
    if (open_modal) {
        modal_data_t* data = (modal_data_t*)open_modal->data;
        if (data && data->content) {
            build_focusable_list_recursive(data->content);
        }
    } else {
        build_focusable_list_recursive(root);
    }

    if (focusable_count > 0) {
        if (prev_focus >= 0 && prev_focus < focusable_count) {
            current_focus_index = prev_focus;
        } else {
            current_focus_index = 0;
        }
        focusable_list[current_focus_index]->focused = true;
    }
}

bool focus_next(void) {
    if (focusable_count == 0) {
        return false;
    }

    if (current_focus_index >= 0) {
        focusable_list[current_focus_index]->focused = false;
    }

    current_focus_index = (current_focus_index + 1) % focusable_count;
    focusable_list[current_focus_index]->focused = true;
    return true;
}

bool focus_prev(void) {
    if (focusable_count == 0) {
        return false;
    }

    if (current_focus_index >= 0) {
        focusable_list[current_focus_index]->focused = false;
    }

    current_focus_index--;
    if (current_focus_index < 0) {
        current_focus_index = focusable_count - 1;
    }

    focusable_list[current_focus_index]->focused = true;
    return true;
}

struct component_t* focus_get_current(void) {
    if (current_focus_index >= 0 && current_focus_index < focusable_count) {
        return focusable_list[current_focus_index];
    }
    return NULL;
}

void focus_clear(void) {
    memset(focusable_list, 0, sizeof(focusable_list));
    focusable_count = 0;
    current_focus_index = -1;
}
