#include "../include/intuitive.h"
#include "internal/component.h"
#include <stdlib.h>
#include <string.h>

/**
 * Create a Text component
 * Returns a new component displaying the given text
 */
component_t* Text(const char* str) {
    if (!str) {
        return NULL;
    }

    // Create component
    component_t* component = component_create(COMPONENT_TEXT);
    if (!component) {
        return NULL;
    }

    // Allocate and set text data
    text_data_t* data = malloc(sizeof(text_data_t));
    if (!data) {
        component_free(component);
        return NULL;
    }

    data->content = strdup(str);
    if (!data->content) {
        free(data);
        component_free(component);
        return NULL;
    }

    component_set_data(component, data);

    return component;
}
