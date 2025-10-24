#include "intuitive.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

char last_key[256] = "";
int last_code = 0;

void dummy(void) {
    // Just for testing
}

component_t* app(void) {
    char info[512];
    snprintf(info, sizeof(info), "Last key code: %d (char: '%c') [%s]",
             last_code,
             (last_code >= 32 && last_code < 127) ? last_code : '?',
             last_key);

    return VStack(
        Text("Key Test - Press keys to see codes", NULL),
        Text("", NULL),
        Text(info, NULL),
        Text("", NULL),
        Button("Test Button", dummy),
        Text("", NULL),
        Text("Press 'q' to quit", NULL),
        NULL
    );
}

int main(void) {
    // Hook into the event system by modifying tui_request_render
    tui_init();
    tui_set_root(app);
    tui_run();
    return 0;
}
