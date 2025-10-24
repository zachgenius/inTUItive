#include "../include/intuitive.h"
#include "internal/component.h"
#include <stdlib.h>
#include <string.h>

component_t* Input(InputConfig config) {
    if (!config.buffer || config.size == 0) {
        return NULL;
    }

    component_t* component = component_create(COMPONENT_INPUT);
    if (!component) {
        return NULL;
    }

    input_data_t* data = malloc(sizeof(input_data_t));
    if (!data) {
        component_free(component);
        return NULL;
    }

    data->buffer = config.buffer;
    data->buffer_size = config.size;
    data->cursor_pos = strlen(config.buffer);
    data->scroll_offset = 0;

    component_set_data(component, data);
    component->focusable = true;

    return component;
}
