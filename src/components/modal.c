#include "intuitive.h"
#include "internal/component.h"
#include <stdlib.h>
#include <string.h>

component_t* Modal(bool* is_open, const char* title, component_t* content, void (*on_close)(void)) {
    if (!is_open || !content) {
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

    data->is_open = is_open;
    data->title = title ? strdup(title) : NULL;
    data->content = content;
    data->on_close = on_close;

    component_set_data(modal, data);
    return modal;
}
