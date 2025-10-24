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
        FgColor(Text("1. Horizontal Alignment in VStack"), COLOR_BRIGHT_YELLOW),
        Text(""),

        Text("Left aligned (default):"),
        AlignedVStack((StackConfig){
            .children = (component_t*[]){
                Text("Short"),
                Text("Medium text"),
                Text("This is a longer piece of text"),
                NULL
            },
            .alignment = ALIGN_START,
            .spacing = 0
        }),
        Text(""),

        Text("Center aligned:"),
        AlignedVStack((StackConfig){
            .children = (component_t*[]){
                Text("Short"),
                Text("Medium text"),
                Text("This is a longer piece of text"),
                NULL
            },
            .alignment = ALIGN_CENTER,
            .spacing = 0
        }),
        Text(""),

        Text("Right aligned:"),
        AlignedVStack((StackConfig){
            .children = (component_t*[]){
                Text("Short"),
                Text("Medium text"),
                Text("This is a longer piece of text"),
                NULL
            },
            .alignment = ALIGN_END,
            .spacing = 0
        }),
        Text(""),

        // Section 2: Spacing
        FgColor(Text("2. Spacing Between Children"), COLOR_BRIGHT_YELLOW),
        Text(""),

        Text("No spacing:"),
        VStack(
            Text("[Box 1]"),
            Text("[Box 2]"),
            Text("[Box 3]"),
            NULL
        ),
        Text(""),

        Text("Spacing = 1:"),
        AlignedVStack((StackConfig){
            .children = (component_t*[]){
                Text("[Box 1]"),
                Text("[Box 2]"),
                Text("[Box 3]"),
                NULL
            },
            .alignment = ALIGN_START,
            .spacing = 1
        }),
        Text(""),

        // Section 3: Padding
        FgColor(Text("3. Padding"), COLOR_BRIGHT_YELLOW),
        Text(""),

        Text("No padding:"),
        FgColor(BgColor(Text("Content"), COLOR_BLUE), COLOR_WHITE),
        Text(""),

        Text("Padding all sides (2):"),
        Padded(
            FgColor(BgColor(Text("Content"), COLOR_BLUE), COLOR_WHITE),
            (PaddingConfig){ .top = 2, .bottom = 2, .left = 2, .right = 2 }
        ),
        Text(""),

        Text("Padding left/right only:"),
        Padded(
            FgColor(BgColor(Text("Content"), COLOR_BLUE), COLOR_WHITE),
            (PaddingConfig){ .top = 0, .bottom = 0, .left = 4, .right = 4 }
        ),
        Text(""),

        // Section 4: HStack alignment
        FgColor(Text("4. Vertical Alignment in HStack"), COLOR_BRIGHT_YELLOW),
        Text(""),

        Text("Top aligned (default):"),
        AlignedHStack((StackConfig){
            .children = (component_t*[]){
                VStack(Text("A"), Text("B"), Text("C"), NULL),
                VStack(Text("1"), NULL),
                VStack(Text("X"), Text("Y"), NULL),
                NULL
            },
            .alignment = ALIGN_START,
            .spacing = 2
        }),
        Text(""),

        Text("Center aligned:"),
        AlignedHStack((StackConfig){
            .children = (component_t*[]){
                VStack(Text("A"), Text("B"), Text("C"), NULL),
                VStack(Text("1"), NULL),
                VStack(Text("X"), Text("Y"), NULL),
                NULL
            },
            .alignment = ALIGN_CENTER,
            .spacing = 2
        }),
        Text(""),

        Text("Bottom aligned:"),
        AlignedHStack((StackConfig){
            .children = (component_t*[]){
                VStack(Text("A"), Text("B"), Text("C"), NULL),
                VStack(Text("1"), NULL),
                VStack(Text("X"), Text("Y"), NULL),
                NULL
            },
            .alignment = ALIGN_END,
            .spacing = 2
        }),
        Text(""),
        NULL
    );

    return VStack(
        Bold(FgColor(Text("=== Layout System Demo ==="), COLOR_BRIGHT_CYAN)),
        Text(""),
        FgColor(Text("Use ↑↓ arrow keys or mouse wheel to scroll"), COLOR_BRIGHT_YELLOW),
        Text(""),
        ScrollView(content, &state.scroll_offset, (ScrollConfig){
            .max_height = viewport_height,
            .show_indicators = true
        }),
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
