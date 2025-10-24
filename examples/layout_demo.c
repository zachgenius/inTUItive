#include "intuitive.h"

typedef struct {
    int scroll_offset;
} app_state_t;

static app_state_t state = {0};

static component_t* app(void) {
    int term_width, term_height;
    tui_get_terminal_size(&term_width, &term_height);

    // Reserve space for title and instructions (4 lines)
    int viewport_height = term_height - 6;

    component_t* content = VStack(
        // Section 1: Alignment
        Text("1. Horizontal Alignment in VStack", (TextConfig){
            .fg_color = COLOR_BRIGHT_YELLOW
        }),
        Text("", TEXT_DEFAULT),

        Text("Left aligned (default):", TEXT_DEFAULT),
        AlignedVStack((StackConfig){
            .children = (component_t*[]){
                Text("Short", TEXT_DEFAULT),
                Text("Medium text", TEXT_DEFAULT),
                Text("This is a longer piece of text", TEXT_DEFAULT),
                NULL
            },
            .alignment = ALIGN_START,
            .spacing = 0
        }),
        Text("", TEXT_DEFAULT),

        Text("Center aligned:", TEXT_DEFAULT),
        AlignedVStack((StackConfig){
            .children = (component_t*[]){
                Text("Short", TEXT_DEFAULT),
                Text("Medium text", TEXT_DEFAULT),
                Text("This is a longer piece of text", TEXT_DEFAULT),
                NULL
            },
            .alignment = ALIGN_CENTER,
            .spacing = 0
        }),
        Text("", TEXT_DEFAULT),

        Text("Right aligned:", TEXT_DEFAULT),
        AlignedVStack((StackConfig){
            .children = (component_t*[]){
                Text("Short", TEXT_DEFAULT),
                Text("Medium text", TEXT_DEFAULT),
                Text("This is a longer piece of text", TEXT_DEFAULT),
                NULL
            },
            .alignment = ALIGN_END,
            .spacing = 0
        }),
        Text("", TEXT_DEFAULT),

        // Section 2: Spacing
        Text("2. Spacing Between Children", (TextConfig){
            .fg_color = COLOR_BRIGHT_YELLOW
        }),
        Text("", TEXT_DEFAULT),

        Text("No spacing:", TEXT_DEFAULT),
        VStack(
            Text("[Box 1]", TEXT_DEFAULT),
            Text("[Box 2]", TEXT_DEFAULT),
            Text("[Box 3]", TEXT_DEFAULT),
            NULL
        ),
        Text("", TEXT_DEFAULT),

        Text("Spacing = 1:", TEXT_DEFAULT),
        AlignedVStack((StackConfig){
            .children = (component_t*[]){
                Text("[Box 1]", TEXT_DEFAULT),
                Text("[Box 2]", TEXT_DEFAULT),
                Text("[Box 3]", TEXT_DEFAULT),
                NULL
            },
            .alignment = ALIGN_START,
            .spacing = 1
        }),
        Text("", TEXT_DEFAULT),

        // Section 3: Padding
        Text("3. Padding", (TextConfig){
            .fg_color = COLOR_BRIGHT_YELLOW
        }),
        Text("", TEXT_DEFAULT),

        Text("No padding:", TEXT_DEFAULT),
        Text("Content", (TextConfig){
            .fg_color = COLOR_WHITE,
            .bg_color = COLOR_BLUE
        }),
        Text("", TEXT_DEFAULT),

        Text("Padding all sides (2):", TEXT_DEFAULT),
        Padded(
            Text("Content", (TextConfig){
                .fg_color = COLOR_WHITE,
                .bg_color = COLOR_BLUE
            }),
            (PaddingConfig){ .top = 2, .bottom = 2, .left = 2, .right = 2 }
        ),
        Text("", TEXT_DEFAULT),

        Text("Padding left/right only:", TEXT_DEFAULT),
        Padded(
            Text("Content", (TextConfig){
                .fg_color = COLOR_WHITE,
                .bg_color = COLOR_BLUE
            }),
            (PaddingConfig){ .top = 0, .bottom = 0, .left = 4, .right = 4 }
        ),
        Text("", TEXT_DEFAULT),

        // Section 4: HStack alignment
        Text("4. Vertical Alignment in HStack", (TextConfig){
            .fg_color = COLOR_BRIGHT_YELLOW
        }),
        Text("", TEXT_DEFAULT),

        Text("Top aligned (default):", TEXT_DEFAULT),
        AlignedHStack((StackConfig){
            .children = (component_t*[]){
                VStack(Text("A", TEXT_DEFAULT), Text("B", TEXT_DEFAULT), Text("C", TEXT_DEFAULT), NULL ),
                VStack(Text("1", TEXT_DEFAULT), NULL ),
                VStack(Text("X", TEXT_DEFAULT), Text("Y", TEXT_DEFAULT), NULL ),
                NULL
            },
            .alignment = ALIGN_START,
            .spacing = 2
        }),
        Text("", TEXT_DEFAULT),

        Text("Center aligned:", TEXT_DEFAULT),
        AlignedHStack((StackConfig){
            .children = (component_t*[]){
                VStack(Text("A", TEXT_DEFAULT), Text("B", TEXT_DEFAULT), Text("C", TEXT_DEFAULT), NULL ),
                VStack(Text("1", TEXT_DEFAULT), NULL ),
                VStack(Text("X", TEXT_DEFAULT), Text("Y", TEXT_DEFAULT), NULL ),
                NULL
            },
            .alignment = ALIGN_CENTER,
            .spacing = 2
        }),
        Text("", TEXT_DEFAULT),

        Text("Bottom aligned:", TEXT_DEFAULT),
        AlignedHStack((StackConfig){
            .children = (component_t*[]){
                VStack(Text("A", TEXT_DEFAULT), Text("B", TEXT_DEFAULT), Text("C", TEXT_DEFAULT), NULL ),
                VStack(Text("1", TEXT_DEFAULT), NULL ),
                VStack(Text("X", TEXT_DEFAULT), Text("Y", TEXT_DEFAULT), NULL ),
                NULL
            },
            .alignment = ALIGN_END,
            .spacing = 2
        }),
        Text("", TEXT_DEFAULT),
        NULL
    );

    return VStack(
        Text("=== Layout System Demo ===", (TextConfig){
            .fg_color = COLOR_BRIGHT_CYAN,
            .style = STYLE_BOLD
        }),
        Text("", TEXT_DEFAULT),
        Text("Use ↑↓ arrow keys or mouse wheel to scroll", (TextConfig){
            .fg_color = COLOR_BRIGHT_YELLOW
        }),
        Text("", TEXT_DEFAULT),
        ScrollView(content, &state.scroll_offset, (ScrollConfig){
            .max_height = viewport_height,
            .show_indicators = true,
            .show_arrows = true
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
