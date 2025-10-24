#include "intuitive.h"
#include "internal/component.h"
#include <stdlib.h>
#include <string.h>

component_t* Modal(ModalConfig config) {
    if (!config.is_open || !config.content) {
        return NULL;
    }

    component_t* modal = component_create(COMPONENT_MODAL);
    if (!modal) {
        return NULL;
    }

    modal_data_t* data = calloc(1, sizeof(modal_data_t));
    if (!data) {
        component_free(modal);
        return NULL;
    }

    data->is_open = config.is_open;
    data->title = config.title ? strdup(config.title) : NULL;
    data->content = config.content;
    data->on_close = config.on_close;

    component_set_data(modal, data);
    return modal;
}
