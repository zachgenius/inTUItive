#include "intuitive.h"

typedef struct {
    int scroll1;
    int scroll2;
    int scroll3;
} app_state_t;

static app_state_t state = {0};

static component_t* create_sample_content(void) {
    return VStack(
        Text("Line 1", TEXT_DEFAULT),
        Text("Line 2", TEXT_DEFAULT),
        Text("Line 3", TEXT_DEFAULT),
        Text("Line 4", TEXT_DEFAULT),
        Text("Line 5", TEXT_DEFAULT),
        Text("Line 6", TEXT_DEFAULT),
        Text("Line 7", TEXT_DEFAULT),
        Text("Line 8", TEXT_DEFAULT),
        Text("Line 9", TEXT_DEFAULT),
        Text("Line 10", TEXT_DEFAULT),
        Text("Line 11", TEXT_DEFAULT),
        Text("Line 12", TEXT_DEFAULT),
        Text("Line 13", TEXT_DEFAULT),
        Text("Line 14", TEXT_DEFAULT),
        Text("Line 15", TEXT_DEFAULT),
        NULL
    );
}

static component_t* app(void) {
    return VStack(
        Text("=== ScrollBar Customization Demo ===", (TextConfig){
            .fg_color = COLOR_BRIGHT_CYAN,
            .style = STYLE_BOLD
        }),
        Text("", TEXT_DEFAULT),
        Text("Tab between scroll views, use ↑↓ or mouse wheel to scroll", (TextConfig){
            .fg_color = COLOR_BRIGHT_YELLOW
        }),
        Text("", TEXT_DEFAULT),

        // Default style
        HStack(
            VStack(
                Text("Default Style:", (TextConfig){ .style = STYLE_BOLD }),
                Text("█ / ▓ / │ + arrows", TEXT_DEFAULT),
                Text("", TEXT_DEFAULT),
                ScrollView(create_sample_content(), &state.scroll1, (ScrollConfig){
                    .max_height = 8,
                    .show_indicators = true,
                    .show_arrows = true
                }),
                NULL
            ),
            Text("    ", TEXT_DEFAULT),

            // Square style
            VStack(
                Text("Square Style:", (TextConfig){ .style = STYLE_BOLD }),
                Text("■ / □ / ┆ no arrows", TEXT_DEFAULT),
                Text("", TEXT_DEFAULT),
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
            Text("    ", TEXT_DEFAULT),

            // Circle style
            VStack(
                Text("Circle Style:", (TextConfig){ .style = STYLE_BOLD }),
                Text("● / ○ / ┊ + arrows", TEXT_DEFAULT),
                Text("", TEXT_DEFAULT),
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
        Text("", TEXT_DEFAULT),

        Text("Other options you can try:", TEXT_DEFAULT),
        Text("  • thumb_focused: █ ■ ● ◆ ▮ ▪ ◼", (TextConfig){
            .fg_color = COLOR_BRIGHT_GREEN
        }),
        Text("  • thumb_unfocused: ▓ □ ○ ◇ ▯ ▫ ◻", (TextConfig){
            .fg_color = COLOR_BRIGHT_GREEN
        }),
        Text("  • track_char: │ ┆ ┊ ╎ ╏ ║ |", (TextConfig){
            .fg_color = COLOR_BRIGHT_GREEN
        }),
        Text("", TEXT_DEFAULT),
        Text("Press 'q' to quit", (TextConfig){ .fg_color = COLOR_BRIGHT_BLACK }),
        NULL
    );
}

int main(void) {
    tui_init();
    tui_set_root(app);
    tui_run();
    return 0;
}
