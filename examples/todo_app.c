#include "intuitive.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define MAX_TODOS 20
#define TODO_LENGTH 256

typedef struct {
    char items[MAX_TODOS][TODO_LENGTH];
    int count;
    char input_buffer[TODO_LENGTH];
} todo_state_t;

todo_state_t state = {{0}, 0, {0}};

void add_todo(void) {
    if (strlen(state.input_buffer) > 0 && state.count < MAX_TODOS) {
        strncpy(state.items[state.count], state.input_buffer, TODO_LENGTH - 1);
        state.items[state.count][TODO_LENGTH - 1] = '\0';
        state.count++;
        state.input_buffer[0] = '\0';
        tui_request_render();
    }
}

void clear_all(void) {
    state.count = 0;
    tui_request_render();
}

component_t* app(void) {
    char header[64];
    snprintf(header, sizeof(header), "TODO List (%d/%d items)", state.count, MAX_TODOS);

    component_t* items[MAX_TODOS + 20];
    int idx = 0;

    items[idx++] = Text(header, TEXT_DEFAULT);
    items[idx++] = Text("", TEXT_DEFAULT);

    items[idx++] = HStack(
        Text("Add: ", TEXT_DEFAULT),
        Input((InputConfig){
            .buffer = state.input_buffer,
            .size = sizeof(state.input_buffer)
        }),
        Text(" ", TEXT_DEFAULT),
        Button("Add", add_todo),
        NULL
    );

    items[idx++] = Text("", TEXT_DEFAULT);

    if (state.count > 0) {
        items[idx++] = Text("Items:", TEXT_DEFAULT);
        for (int i = 0; i < state.count; i++) {
            char item_text[TODO_LENGTH + 10];
            snprintf(item_text, sizeof(item_text), "  %d. %s", i + 1, state.items[i]);
            items[idx++] = Text(item_text, TEXT_DEFAULT);
        }

        items[idx++] = Text("", TEXT_DEFAULT);
        items[idx++] = Button("Clear All", clear_all);
    } else {
        items[idx++] = Text("No items yet. Add one above!", TEXT_DEFAULT);
    }

    items[idx++] = Text("", TEXT_DEFAULT);
    items[idx++] = Text("Controls:", TEXT_DEFAULT);
    items[idx++] = Text("  Tab: Navigate between fields", TEXT_DEFAULT);
    items[idx++] = Text("  Enter: Add item / Activate button", TEXT_DEFAULT);
    items[idx++] = Text("  q: Quit", TEXT_DEFAULT);
    items[idx] = NULL;

    return VStackArray(items);
}

int main(void) {
    tui_init();
    tui_set_root(app);
    tui_run();
    return 0;
}
