#include "intuitive.h"
#include <stdio.h>

component_t* app(void) {
    return VStack(
        Text("inTUItive Styling Demo", &(TextConfig){
            .style = STYLE_BOLD
        }),
        Text("", NULL),

        Text("=== Colors ===", NULL),
        Text("Red text", &(TextConfig){ .fg_color = COLOR_RED }),
        Text("Green text", &(TextConfig){ .fg_color = COLOR_GREEN }),
        Text("Blue text", &(TextConfig){ .fg_color = COLOR_BLUE }),
        Text("Yellow text", &(TextConfig){ .fg_color = COLOR_YELLOW }),
        Text("Magenta text", &(TextConfig){ .fg_color = COLOR_MAGENTA }),
        Text("Cyan text", &(TextConfig){ .fg_color = COLOR_CYAN }),
        Text("", NULL),

        Text("=== Bright Colors ===", NULL),
        Text("Bright Red text", &(TextConfig){ .fg_color = COLOR_BRIGHT_RED }),
        Text("Bright Green text", &(TextConfig){ .fg_color = COLOR_BRIGHT_GREEN }),
        Text("Bright Blue text", &(TextConfig){ .fg_color = COLOR_BRIGHT_BLUE }),
        Text("Bright Yellow text", &(TextConfig){ .fg_color = COLOR_BRIGHT_YELLOW }),
        Text("", NULL),

        Text("=== Styles ===", NULL),
        Text("Bold text", &(TextConfig){ .style = STYLE_BOLD }),
        Text("Underlined text", &(TextConfig){ .style = STYLE_UNDERLINE }),
        Text("Bold and underlined", &(TextConfig){ .style = STYLE_BOLD | STYLE_UNDERLINE }),
        Text("", NULL),

        Text("=== Combined Styling ===", NULL),
        Text("Bold Red Error", &(TextConfig){
            .fg_color = COLOR_RED,
            .style = STYLE_BOLD
        }),
        Text("Bold Green Success", &(TextConfig){
            .fg_color = COLOR_GREEN,
            .style = STYLE_BOLD
        }),
        Text("Underlined Blue Link", &(TextConfig){
            .fg_color = COLOR_BLUE,
            .style = STYLE_UNDERLINE
        }),
        Text("", NULL),

        Text("=== Background Colors ===", NULL),
        Text(" White on Red ", &(TextConfig){
            .fg_color = COLOR_WHITE,
            .bg_color = COLOR_RED
        }),
        Text(" Black on Yellow ", &(TextConfig){
            .fg_color = COLOR_BLACK,
            .bg_color = COLOR_YELLOW
        }),
        Text(" White on Blue ", &(TextConfig){
            .fg_color = COLOR_WHITE,
            .bg_color = COLOR_BLUE
        }),
        Text("", NULL),

        Text("Press 'q' to quit", NULL),
        NULL
    );
}

int main(void) {
    tui_init();
    tui_set_root(app);
    tui_run();
    return 0;
}
