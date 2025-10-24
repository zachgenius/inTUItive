#include "intuitive.h"

typedef struct {
    int scroll1;
    int scroll2;
    int scroll3;
} app_state_t;

static app_state_t state = {0};

static component_t* create_sample_content(void) {
    return VStack(
        Text("Line 1", NULL),
        Text("Line 2", NULL),
        Text("Line 3", NULL),
        Text("Line 4", NULL),
        Text("Line 5", NULL),
        Text("Line 6", NULL),
        Text("Line 7", NULL),
        Text("Line 8", NULL),
        Text("Line 9", NULL),
        Text("Line 10", NULL),
        Text("Line 11", NULL),
        Text("Line 12", NULL),
        Text("Line 13", NULL),
        Text("Line 14", NULL),
        Text("Line 15", NULL),
        NULL
    );
}

static component_t* app(void) {
    return VStack(
        Text("=== ScrollBar Customization Demo ===", &(TextConfig){
            .fg_color = COLOR_BRIGHT_CYAN,
            .style = STYLE_BOLD
        }),
        Text("", NULL),
        Text("Tab between scroll views, use ↑↓ or mouse wheel to scroll", &(TextConfig){
            .fg_color = COLOR_BRIGHT_YELLOW
        }),
        Text("", NULL),

        // Default style
        HStack(
            VStack(
                Text("Default Style:", &(TextConfig){ .style = STYLE_BOLD }),
                Text("█ / ▓ / │ + arrows", NULL),
                Text("", NULL),
                ScrollView(create_sample_content(), &state.scroll1, (ScrollConfig){
                    .max_height = 8,
                    .show_indicators = true,
                    .show_arrows = true
                }),
                NULL
            ),
            Text("    ", NULL),

            // Square style
            VStack(
                Text("Square Style:", &(TextConfig){ .style = STYLE_BOLD }),
                Text("■ / □ / ┆ no arrows", NULL),
                Text("", NULL),
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
            Text("    ", NULL),

            // Circle style
            VStack(
                Text("Circle Style:", &(TextConfig){ .style = STYLE_BOLD }),
                Text("● / ○ / ┊ + arrows", NULL),
                Text("", NULL),
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
        Text("", NULL),

        Text("Other options you can try:", NULL),
        Text("  • thumb_focused: █ ■ ● ◆ ▮ ▪ ◼", &(TextConfig){
            .fg_color = COLOR_BRIGHT_GREEN
        }),
        Text("  • thumb_unfocused: ▓ □ ○ ◇ ▯ ▫ ◻", &(TextConfig){
            .fg_color = COLOR_BRIGHT_GREEN
        }),
        Text("  • track_char: │ ┆ ┊ ╎ ╏ ║ |", &(TextConfig){
            .fg_color = COLOR_BRIGHT_GREEN
        }),
        Text("", NULL),
        Text("Press 'q' to quit", &(TextConfig){ .fg_color = COLOR_BRIGHT_BLACK }),
        NULL
    );
}

int main(void) {
    tui_init();
    tui_set_root(app);
    tui_run();
    return 0;
}
