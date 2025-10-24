#include "intuitive.h"

typedef struct {
    int scroll1;
    int scroll2;
    int scroll3;
} app_state_t;

static app_state_t state = {0};

static component_t* create_sample_content(void) {
    return VStack(
        Text("Line 1"),
        Text("Line 2"),
        Text("Line 3"),
        Text("Line 4"),
        Text("Line 5"),
        Text("Line 6"),
        Text("Line 7"),
        Text("Line 8"),
        Text("Line 9"),
        Text("Line 10"),
        Text("Line 11"),
        Text("Line 12"),
        Text("Line 13"),
        Text("Line 14"),
        Text("Line 15"),
        NULL
    );
}

static component_t* app(void) {
    return VStack(
        Bold(FgColor(Text("=== ScrollBar Customization Demo ==="), COLOR_BRIGHT_CYAN)),
        Text(""),
        FgColor(Text("Tab between scroll views, use ↑↓ or mouse wheel to scroll"), COLOR_BRIGHT_YELLOW),
        Text(""),

        // Default style
        HStack(
            VStack(
                Bold(Text("Default Style:")),
                Text("█ / ▓ / │ + arrows"),
                Text(""),
                ScrollView(create_sample_content(), &state.scroll1, (ScrollConfig){
                    .max_height = 8,
                    .show_indicators = true,
                    .show_arrows = true
                }),
                NULL
            ),
            Text("    "),

            // Square style
            VStack(
                Bold(Text("Square Style:")),
                Text("■ / □ / ┆ no arrows"),
                Text(""),
                ScrollView(create_sample_content(), &state.scroll2, (ScrollConfig){
                    .max_height = 8,
                    .show_indicators = true,
                    .thumb_focused = "■",
                    .thumb_unfocused = "□",
                    .track_char = "┆",
                    .show_arrows = false
                }),
                NULL
            ),
            Text("    "),

            // Circle style
            VStack(
                Bold(Text("Circle Style:")),
                Text("● / ○ / ┊ + arrows"),
                Text(""),
                ScrollView(create_sample_content(), &state.scroll3, (ScrollConfig){
                    .max_height = 8,
                    .show_indicators = true,
                    .thumb_focused = "●",
                    .thumb_unfocused = "○",
                    .track_char = "┊",
                    .show_arrows = true
                }),
                NULL
            ),
            NULL
        ),
        Text(""),

        Text("Other options you can try:"),
        FgColor(Text("  • thumb_focused: █ ■ ● ◆ ▮ ▪ ◼"), COLOR_BRIGHT_GREEN),
        FgColor(Text("  • thumb_unfocused: ▓ □ ○ ◇ ▯ ▫ ◻"), COLOR_BRIGHT_GREEN),
        FgColor(Text("  • track_char: │ ┆ ┊ ╎ ╏ ║ |"), COLOR_BRIGHT_GREEN),
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
