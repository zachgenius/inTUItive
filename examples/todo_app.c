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

    items[idx++] = Text(header);
    items[idx++] = Text("");

    items[idx++] = HStack(
        Text("Add: "),
        Input(state.input_buffer, sizeof(state.input_buffer)),
        Text(" "),
        Button("Add", add_todo),
        NULL
    );

    items[idx++] = Text("");

    if (state.count > 0) {
        items[idx++] = Text("Items:");
        for (int i = 0; i < state.count; i++) {
            char item_text[TODO_LENGTH + 10];
            snprintf(item_text, sizeof(item_text), "  %d. %s", i + 1, state.items[i]);
            items[idx++] = Text(item_text);
        }

        items[idx++] = Text("");
        items[idx++] = Button("Clear All", clear_all);
    } else {
        items[idx++] = Text("No items yet. Add one above!");
    }

    items[idx++] = Text("");
    items[idx++] = Text("Controls:");
    items[idx++] = Text("  Tab: Navigate between fields");
    items[idx++] = Text("  Enter: Add item / Activate button");
    items[idx++] = Text("  q: Quit");
    items[idx] = NULL;

    return VStackArray(items);
}

int main(void) {
    tui_init();
    tui_set_root(app);
    tui_run();
    return 0;
}
