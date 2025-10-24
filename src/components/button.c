#include "../include/intuitive.h"
#include "internal/component.h"
#include <stdlib.h>
#include <string.h>

component_t* Button(const char* label, void (*on_click)(void)) {
    if (!label) {
        return NULL;
    }

    component_t* component = component_create(COMPONENT_BUTTON);
    if (!component) {
        return NULL;
    }

    button_data_t* data = malloc(sizeof(button_data_t));
    if (!data) {
        component_free(component);
        return NULL;
    }

    data->label = strdup(label);
    if (!data->label) {
        free(data);
        component_free(component);
        return NULL;
    }

    data->on_click = on_click;
    component_set_data(component, data);
    component->focusable = true;

    return component;
}
