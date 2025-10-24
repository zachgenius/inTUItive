#include "intuitive.h"
#include "internal/component.h"
#include <stdlib.h>

component_t* ScrollView(component_t* content, int* scroll_offset, ScrollConfig config) {
    if (!content || !scroll_offset || config.max_height <= 0) {
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
    data->max_visible_height = config.max_height;
    data->show_indicators = config.show_indicators;

    // Set scroll bar appearance with defaults if not specified
    data->thumb_focused = config.thumb_focused ? config.thumb_focused : "█";
    data->thumb_unfocused = config.thumb_unfocused ? config.thumb_unfocused : "▓";
    data->track_char = config.track_char ? config.track_char : "│";
    data->show_arrows = config.show_arrows;

    component_set_data(scrollview, data);

    // Make ScrollView focusable so it can receive arrow key events
    scrollview->focusable = true;

    return scrollview;
}
