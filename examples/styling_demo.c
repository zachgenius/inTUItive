#include "intuitive.h"
#include <stdio.h>

component_t* app(void) {
    return VStack(
        Bold(Text("inTUItive Styling Demo")),
        Text(""),

        Text("=== Colors ==="),
        FgColor(Text("Red text"), COLOR_RED),
        FgColor(Text("Green text"), COLOR_GREEN),
        FgColor(Text("Blue text"), COLOR_BLUE),
        FgColor(Text("Yellow text"), COLOR_YELLOW),
        FgColor(Text("Magenta text"), COLOR_MAGENTA),
        FgColor(Text("Cyan text"), COLOR_CYAN),
        Text(""),

        Text("=== Bright Colors ==="),
        FgColor(Text("Bright Red text"), COLOR_BRIGHT_RED),
        FgColor(Text("Bright Green text"), COLOR_BRIGHT_GREEN),
        FgColor(Text("Bright Blue text"), COLOR_BRIGHT_BLUE),
        FgColor(Text("Bright Yellow text"), COLOR_BRIGHT_YELLOW),
        Text(""),

        Text("=== Styles ==="),
        Bold(Text("Bold text")),
        Underline(Text("Underlined text")),
        Bold(Underline(Text("Bold and underlined"))),
        Text(""),

        Text("=== Combined ==="),
        Bold(FgColor(Text("Bold Red Error"), COLOR_RED)),
        Bold(FgColor(Text("Bold Green Success"), COLOR_GREEN)),
        Underline(FgColor(Text("Underlined Blue Link"), COLOR_BLUE)),
        Text(""),

        Text("=== Background Colors ==="),
        Color(Text(" White on Red "), COLOR_WHITE, COLOR_RED),
        Color(Text(" Black on Yellow "), COLOR_BLACK, COLOR_YELLOW),
        Color(Text(" White on Blue "), COLOR_WHITE, COLOR_BLUE),
        Text(""),

        Text("Press 'q' to quit"),
        NULL
    );
}

int main(void) {
    tui_init();
    tui_set_root(app);
    tui_run();
    return 0;
}
