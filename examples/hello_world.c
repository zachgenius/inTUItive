#include "intuitive.h"
#include <stddef.h>

/**
 * Hello World example for inTUItive
 * Demonstrates basic Text, VStack, and HStack components
 */

component_t* app(void) {
    return VStack(
        Text("Hello, inTUItive!", TEXT_DEFAULT),
        Text("Welcome to declarative TUI", TEXT_DEFAULT),
        Text("", TEXT_DEFAULT),
        HStack(
            Text("[", TEXT_DEFAULT),
            Text("Press 'q' to quit", TEXT_DEFAULT),
            Text("]", TEXT_DEFAULT),
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
