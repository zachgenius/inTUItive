#include "intuitive.h"
#include <stddef.h>
#include <stdio.h>

typedef struct {
    int count;
} counter_state_t;

counter_state_t state = {0};

void increment(void) {
    state.count++;
    tui_request_render();
}

void decrement(void) {
    state.count--;
    tui_request_render();
}

void reset(void) {
    state.count = 0;
    tui_request_render();
}

component_t* app(void) {
    char count_text[64];
    snprintf(count_text, sizeof(count_text), "Count: %d", state.count);

    return VStack(
        Text("Counter Example"),
        Text(""),
        Text(count_text),
        Text(""),
        HStack(
            Button("-", decrement),
            Text("  "),
            Button("+", increment),
            Text("  "),
            Button("Reset", reset),
            NULL
        ),
        Text(""),
        Text("Press Tab to navigate, Enter to activate"),
        Text("Press 'q' to quit"),
        NULL
    );
}

int main(void) {
    tui_init();
    tui_set_root(app);
    tui_run();
    return 0;
}
