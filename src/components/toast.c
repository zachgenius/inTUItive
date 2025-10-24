#include "../internal/component.h"
#include "../internal/animation.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

component_t* Toast(ToastConfig config) {
    if (!config.message || !config.is_visible) {
        return NULL;
    }

    component_t* comp = component_create(COMPONENT_TOAST);
    if (!comp) {
        return NULL;
    }

    toast_data_t* data = (toast_data_t*)malloc(sizeof(toast_data_t));
    if (!data) {
        component_free(comp);
        return NULL;
    }

    data->message = strdup(config.message);
    if (!data->message) {
        free(data);
        component_free(comp);
        return NULL;
    }

    data->is_visible = config.is_visible;
    data->position = config.position;
    data->on_close = config.on_close;

    component_set_data(comp, data);

    return comp;
}
