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
        Text("1. Horizontal Alignment in VStack", &(TextConfig){
            .fg_color = COLOR_BRIGHT_YELLOW
        }),
        Text("", NULL),

        Text("Left aligned (default):", NULL),
        AlignedVStack((StackConfig){
            .children = (component_t*[]){
                Text("Short", NULL),
                Text("Medium text", NULL),
                Text("This is a longer piece of text", NULL),
                NULL
            },
            .alignment = ALIGN_START,
            .spacing = 0
        }),
        Text("", NULL),

        Text("Center aligned:", NULL),
        AlignedVStack((StackConfig){
            .children = (component_t*[]){
                Text("Short", NULL),
                Text("Medium text", NULL),
                Text("This is a longer piece of text", NULL),
                NULL
            },
            .alignment = ALIGN_CENTER,
            .spacing = 0
        }),
        Text("", NULL),

        Text("Right aligned:", NULL),
        AlignedVStack((StackConfig){
            .children = (component_t*[]){
                Text("Short", NULL),
                Text("Medium text", NULL),
                Text("This is a longer piece of text", NULL),
                NULL
            },
            .alignment = ALIGN_END,
            .spacing = 0
        }),
        Text("", NULL),

        // Section 2: Spacing
        Text("2. Spacing Between Children", &(TextConfig){
            .fg_color = COLOR_BRIGHT_YELLOW
        }),
        Text("", NULL),

        Text("No spacing:", NULL),
        VStack(
            Text("[Box 1]", NULL),
            Text("[Box 2]", NULL),
            Text("[Box 3]", NULL),
            NULL
        ),
        Text("", NULL),

        Text("Spacing = 1:", NULL),
        AlignedVStack((StackConfig){
            .children = (component_t*[]){
                Text("[Box 1]", NULL),
                Text("[Box 2]", NULL),
                Text("[Box 3]", NULL),
                NULL
            },
            .alignment = ALIGN_START,
            .spacing = 1
        }),
        Text("", NULL),

        // Section 3: Padding
        Text("3. Padding", &(TextConfig){
            .fg_color = COLOR_BRIGHT_YELLOW
        }),
        Text("", NULL),

        Text("No padding:", NULL),
        Text("Content", &(TextConfig){
            .fg_color = COLOR_WHITE,
            .bg_color = COLOR_BLUE
        }),
        Text("", NULL),

        Text("Padding all sides (2):", NULL),
        Padded(
            Text("Content", &(TextConfig){
                .fg_color = COLOR_WHITE,
                .bg_color = COLOR_BLUE
            }),
            (PaddingConfig){ .top = 2, .bottom = 2, .left = 2, .right = 2 }
        ),
        Text("", NULL),

        Text("Padding left/right only:", NULL),
        Padded(
            Text("Content", &(TextConfig){
                .fg_color = COLOR_WHITE,
                .bg_color = COLOR_BLUE
            }),
            (PaddingConfig){ .top = 0, .bottom = 0, .left = 4, .right = 4 }
        ),
        Text("", NULL),

        // Section 4: HStack alignment
        Text("4. Vertical Alignment in HStack", &(TextConfig){
            .fg_color = COLOR_BRIGHT_YELLOW
        }),
        Text("", NULL),

        Text("Top aligned (default):", NULL),
        AlignedHStack((StackConfig){
            .children = (component_t*[]){
                VStack(Text("A", NULL), Text("B", NULL), Text("C", NULL), NULL ),
                VStack(Text("1", NULL), NULL ),
                VStack(Text("X", NULL), Text("Y", NULL), NULL ),
                NULL
            },
            .alignment = ALIGN_START,
            .spacing = 2
        }),
        Text("", NULL),

        Text("Center aligned:", NULL),
        AlignedHStack((StackConfig){
            .children = (component_t*[]){
                VStack(Text("A", NULL), Text("B", NULL), Text("C", NULL), NULL ),
                VStack(Text("1", NULL), NULL ),
                VStack(Text("X", NULL), Text("Y", NULL), NULL ),
                NULL
            },
            .alignment = ALIGN_CENTER,
            .spacing = 2
        }),
        Text("", NULL),

        Text("Bottom aligned:", NULL),
        AlignedHStack((StackConfig){
            .children = (component_t*[]){
                VStack(Text("A", NULL), Text("B", NULL), Text("C", NULL), NULL ),
                VStack(Text("1", NULL), NULL ),
                VStack(Text("X", NULL), Text("Y", NULL), NULL ),
                NULL
            },
            .alignment = ALIGN_END,
            .spacing = 2
        }),
        Text("", NULL),
        NULL
    );

    return VStack(
        Text("=== Layout System Demo ===", &(TextConfig){
            .fg_color = COLOR_BRIGHT_CYAN,
            .style = STYLE_BOLD
        }),
        Text("", NULL),
        Text("Use ↑↓ arrow keys or mouse wheel to scroll", &(TextConfig){
            .fg_color = COLOR_BRIGHT_YELLOW
        }),
        Text("", NULL),
        ScrollView(content, &state.scroll_offset, (ScrollConfig){
            .max_height = viewport_height,
            .show_indicators = true,
            .show_arrows = true
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
