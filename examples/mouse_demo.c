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
    // Create list items - make it long enough to scroll
    static const char* items[] = {
        "📋 Item 1 - Click me!",
        "📋 Item 2 - Or me!",
        "📋 Item 3 - Try scrolling too!",
        "📋 Item 4 - Scroll with mouse wheel",
        "📋 Item 5 - Or use arrow keys",
        "📋 Item 6 - Mouse and keyboard both work",
        "📋 Item 7 - Keep scrolling...",
        "📋 Item 8 - Still more items below",
        "📋 Item 9 - Getting closer",
        "📋 Item 10 - Halfway there!",
        "📋 Item 11 - More to go",
        "📋 Item 12 - Keep going",
        "📋 Item 13 - Almost done",
        "📋 Item 14 - One more",
        "📋 Item 15 - You found me!",
        "📋 Item 16 - Wait, there's more?",
        "📋 Item 17 - Yes, more items",
        "📋 Item 18 - Keep scrolling",
        "📋 Item 19 - Nearly at the end",
        "📋 Item 20 - This is the last one!",
    };

    char count_str[64];
    snprintf(count_str, sizeof(count_str), "Click count: %d", state.click_count);

    // Show scroll position
    char scroll_info[64];
    snprintf(scroll_info, sizeof(scroll_info),
             "Showing items %d-%d of 20 (scroll: %d)",
             state.list_scroll + 1,
             state.list_scroll + 8 < 20 ? state.list_scroll + 8 : 20,
             state.list_scroll);

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
        FgColor(Text("(Press Tab to focus, then use ↑↓ arrow keys or mouse wheel ▲▼)"), COLOR_BRIGHT_BLACK),
        FgColor(Text(scroll_info), COLOR_YELLOW),
        List((ListConfig){
            .items = items,
            .count = 20,
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
