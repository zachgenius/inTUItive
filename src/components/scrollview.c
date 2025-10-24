#include "intuitive.h"
#include "internal/component.h"
#include <stdlib.h>

component_t* ScrollView(component_t* content, int* scroll_offset, int max_height) {
    if (!content || !scroll_offset || max_height <= 0) {
        return NULL;
    }

    component_t* scrollview = component_create(COMPONENT_SCROLLVIEW);
    if (!scrollview) {
        return NULL;
    }

    scrollview_data_t* data = calloc(1, sizeof(scrollview_data_t));
    if (!data) {
        component_free(scrollview);
        return NULL;
    }

    data->content = content;
    data->scroll_offset = scroll_offset;
    data->max_visible_height = max_height;
    data->show_indicators = true;  // Show indicators by default

    component_set_data(scrollview, data);

    // Make ScrollView focusable so it can receive arrow key events
    scrollview->focusable = true;

    return scrollview;
}

component_t* ScrollIndicators(component_t* comp, bool enabled) {
    if (!comp || comp->type != COMPONENT_SCROLLVIEW) {
        return comp;
    }

    scrollview_data_t* data = (scrollview_data_t*)comp->data;
    if (data) {
        data->show_indicators = enabled;
    }

    return comp;
}
