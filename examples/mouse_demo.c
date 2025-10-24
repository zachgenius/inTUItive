#include "intuitive.h"
#include <stdio.h>

typedef struct {
    int click_count;
    int selected_item;
    int list_scroll;
    char message[128];
} app_state_t;

static app_state_t state = {
    .click_count = 0,
    .selected_item = 0,
    .list_scroll = 0,
    .message = "Welcome! Click the buttons or list items."
};

static void button_clicked(void) {
    state.click_count++;
    snprintf(state.message, sizeof(state.message),
             "Button clicked %d time%s!",
             state.click_count,
             state.click_count == 1 ? "" : "s");
    tui_request_render();
}

static void reset_counter(void) {
    state.click_count = 0;
    snprintf(state.message, sizeof(state.message), "Counter reset!");
    tui_request_render();
}

static void on_item_select(int index) {
    snprintf(state.message, sizeof(state.message),
             "You clicked item #%d", index + 1);
    tui_request_render();
}

static component_t* app(void) {
    // Create list items
    static const char* items[] = {
        "📋 Item 1 - Click me!",
        "📋 Item 2 - Or me!",
        "📋 Item 3 - Try scrolling too!",
        "📋 Item 4",
        "📋 Item 5",
        "📋 Item 6",
        "📋 Item 7",
        "📋 Item 8",
        "📋 Item 9",
        "📋 Item 10",
        "📋 Item 11",
        "📋 Item 12",
        "📋 Item 13",
        "📋 Item 14",
        "📋 Item 15",
    };

    char count_str[64];
    snprintf(count_str, sizeof(count_str), "Click count: %d", state.click_count);

    return VStack(
        Bold(FgColor(Text("=== Mouse Support Demo ==="), COLOR_BRIGHT_CYAN)),
        Text(""),
        FgColor(Text("🖱️  This demo showcases mouse interaction!"), COLOR_BRIGHT_YELLOW),
        Text(""),

        Text("Try clicking the buttons:"),
        HStack(
            Button("Click Me!", button_clicked),
            Text("  "),
            Button("Reset Counter", reset_counter),
            NULL
        ),
        Text(count_str),
        Text(""),

        Text("Try clicking items in the list:"),
        FgColor(Text("(You can also scroll with mouse wheel!)"), COLOR_BRIGHT_BLACK),
        List((ListConfig){
            .items = items,
            .count = 15,
            .max_visible = 8,
            .scroll_offset = &state.list_scroll,
            .selected_index = &state.selected_item,
            .on_select = on_item_select
        }),
        Text(""),

        FgColor(Text(state.message), COLOR_BRIGHT_GREEN),
        Text(""),
        FgColor(Text("Press 'q' to quit"), COLOR_BRIGHT_BLACK),
        NULL
    );
}

int main(void) {
    tui_init();
    tui_set_root(app);
    tui_run();
    return 0;
}
