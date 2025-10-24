#include "intuitive.h"
#include "internal/component.h"
#include <stdlib.h>
#include <string.h>

component_t* List(ListConfig config) {
    if (!config.items || config.count <= 0) {
        return NULL;
    }

    component_t* list = component_create(COMPONENT_LIST);
    if (!list) {
        return NULL;
    }

    list_data_t* data = calloc(1, sizeof(list_data_t));
    if (!data) {
        component_free(list);
        return NULL;
    }

    data->items = calloc(config.count, sizeof(char*));
    if (!data->items) {
        free(data);
        component_free(list);
        return NULL;
    }

    for (int i = 0; i < config.count; i++) {
        data->items[i] = strdup(config.items[i]);
        if (!data->items[i]) {
            for (int j = 0; j < i; j++) {
                free(data->items[j]);
            }
            free(data->items);
            free(data);
            component_free(list);
            return NULL;
        }
    }

    data->item_count = config.count;
    data->scroll_offset = 0;
    data->max_visible_items = config.max_visible > 0 ? config.max_visible : 10;
    data->selected_index = config.selected_index;
    data->on_select = config.on_select;

    component_set_data(list, data);

    // Make list focusable if it has selection support
    if (data->selected_index != NULL) {
        list->focusable = true;
    }

    return list;
}
