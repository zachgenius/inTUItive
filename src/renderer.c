#include "internal/renderer.h"
#include "internal/component.h"
#include "internal/terminal.h"

void render_component(struct component_t* component) {
    if (!component) {
        return;
    }

    switch (component->type) {
        case COMPONENT_TEXT: {
            text_data_t* data = (text_data_t*)component->data;
            if (data && data->content) {
                term_move_cursor(component->x, component->y);
                term_write(data->content);
            }
            break;
        }

        case COMPONENT_VSTACK:
        case COMPONENT_HSTACK:
            for (int i = 0; i < component->child_count; i++) {
                render_component(component->children[i]);
            }
            break;
    }
}
