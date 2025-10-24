#include "../include/intuitive.h"
#include "internal/component.h"
#include <stdlib.h>

component_t* Padded(component_t* child, PaddingConfig padding) {
    if (!child) {
        return NULL;
    }

    component_t* padded = component_create(COMPONENT_PADDING);
    if (!padded) {
        component_free(child);
        return NULL;
    }

    padding_data_t* data = calloc(1, sizeof(padding_data_t));
    if (!data) {
        component_free(child);
        component_free(padded);
        return NULL;
    }

    data->child = child;
    data->padding = padding;

    component_set_data(padded, data);

    // Copy style from child if it has styling
    if (child->fg_color != COLOR_DEFAULT) {
        padded->fg_color = child->fg_color;
    }
    if (child->bg_color != COLOR_DEFAULT) {
        padded->bg_color = child->bg_color;
    }
    if (child->style != STYLE_NONE) {
        padded->style = child->style;
    }

    return padded;
}
