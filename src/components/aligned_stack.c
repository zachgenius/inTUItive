#include "../include/intuitive.h"
#include "internal/component.h"
#include <stdlib.h>

component_t* AlignedVStack(StackConfig config) {
    component_t* vstack = component_create(COMPONENT_VSTACK);
    if (!vstack) {
        return NULL;
    }

    // Add children
    if (config.children) {
        for (int i = 0; config.children[i] != NULL; i++) {
            component_add_child(vstack, config.children[i]);
        }
    }

    // Set alignment and spacing data
    stack_data_t* data = calloc(1, sizeof(stack_data_t));
    if (!data) {
        component_free(vstack);
        return NULL;
    }

    data->alignment = config.alignment;
    data->spacing = config.spacing;

    component_set_data(vstack, data);

    return vstack;
}

component_t* AlignedHStack(StackConfig config) {
    component_t* hstack = component_create(COMPONENT_HSTACK);
    if (!hstack) {
        return NULL;
    }

    // Add children
    if (config.children) {
        for (int i = 0; config.children[i] != NULL; i++) {
            component_add_child(hstack, config.children[i]);
        }
    }

    // Set alignment and spacing data
    stack_data_t* data = calloc(1, sizeof(stack_data_t));
    if (!data) {
        component_free(hstack);
        return NULL;
    }

    data->alignment = config.alignment;
    data->spacing = config.spacing;

    component_set_data(hstack, data);

    return hstack;
}
