#include "intuitive.h"
#include <stdio.h>

typedef struct {
    int scroll_pos;
} demo_state_t;

demo_state_t state = {0};

component_t* app(void) {
    component_t* items[25];

    for (int i = 0; i < 20; i++) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Line %d - This is some content", i + 1);
        items[i] = Text(buf);
    }
    items[20] = NULL;

    component_t* long_content = VStackArray(items);

    char debug[64];
    snprintf(debug, sizeof(debug), "Scroll position: %d", state.scroll_pos);

    return VStack(
        Bold(FgColor(Text("ScrollView Demo"), COLOR_BRIGHT_CYAN)),
        Text(""),
        Text(debug),
        Text("Content (showing 10 of 20 lines):"),
        ScrollView(long_content, &state.scroll_pos, (ScrollConfig){
            .max_height = 10,
            .show_indicators = true
        }),
        Text(""),
        FgColor(Text("Controls:"), COLOR_BRIGHT_GREEN),
        Text("  Tab - Focus scrollview"),
        Text("  Up/Down arrows - Scroll"),
        Text("  q - Quit"),
        NULL
    );
}

int main(void) {
    tui_init();
    tui_set_root(app);
    tui_run();
    return 0;
}
