#include "intuitive.h"
#include <stddef.h>

/**
 * Hello World example for inTUItive
 * Demonstrates basic Text, VStack, and HStack components
 */

component_t* app(void) {
    return VStack(
        Text("Hello, inTUItive!", NULL),
        Text("Welcome to declarative TUI", NULL),
        Text("", NULL),
        HStack(
            Text("[", NULL),
            Text("Press 'q' to quit", NULL),
            Text("]", NULL),
            NULL
        ),
        NULL
    );
}

int main(void) {
    tui_init();
    tui_set_root(app);
    tui_run();
    return 0;
}
