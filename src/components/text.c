#include "../include/intuitive.h"
#include "internal/component.h"
#include <stdlib.h>
#include <string.h>

component_t* Text(const char* content, TextConfig config) {
    if (!content) {
        return NULL;
    }

    component_t* component = component_create(COMPONENT_TEXT);
    if (!component) {
        return NULL;
    }

    text_data_t* data = malloc(sizeof(text_data_t));
    if (!data) {
        component_free(component);
        return NULL;
    }

    data->content = strdup(content);
    if (!data->content) {
        free(data);
        component_free(component);
        return NULL;
    }

    component_set_data(component, data);

    // Apply styling from config (zero values are treated as defaults)
    component->fg_color = config.fg_color;
    component->bg_color = config.bg_color;
    component->style = config.style;

    return component;
}
