#include "intuitive.h"
#include <stdio.h>

typedef struct {
    bool show_toast_bottom;
    bool show_toast_top;
    bool show_toast_top_right;
    bool show_toast_bottom_right;
} app_state_t;

static app_state_t state = {
    .show_toast_bottom = false,
    .show_toast_top = false,
    .show_toast_top_right = false,
    .show_toast_bottom_right = false,
};

static void show_bottom(void) {
    state.show_toast_bottom = true;
}

static void show_top(void) {
    state.show_toast_top = true;
}

static void show_top_right(void) {
    state.show_toast_top_right = true;
}

static void show_bottom_right(void) {
    state.show_toast_bottom_right = true;
}

static void close_toast(void) {
    // Callback when toast closes (optional)
}

static component_t* app(void) {
    return VStack(
        Text("=== TOAST NOTIFICATION DEMO ===", (TextConfig){
            .fg_color = COLOR_BRIGHT_CYAN,
            .style = STYLE_BOLD
        }),
        Text("", TEXT_DEFAULT),

        Text("Click buttons to show toasts at different positions:", TEXT_DEFAULT),
        Text("Toasts auto-dismiss after 2 seconds with smooth slide animations", (TextConfig){
            .fg_color = COLOR_BRIGHT_BLACK
        }),
        Text("", TEXT_DEFAULT),

        HStack(
            Button("Bottom", show_bottom),
            Text("  ", TEXT_DEFAULT),
            Button("Top", show_top),
            Text("  ", TEXT_DEFAULT),
            Button("Top Right", show_top_right),
            Text("  ", TEXT_DEFAULT),
            Button("Bottom Right", show_bottom_right),
            NULL
        ),

        Text("", TEXT_DEFAULT),
        Text("Tab to navigate, Enter/Space to activate, q to quit", (TextConfig){
            .fg_color = COLOR_BRIGHT_BLACK
        }),

        // Toasts overlay on top of everything
        // Always add toasts - they only render when is_visible is true
        Toast((ToastConfig){
            .message = "File saved successfully!",
            .is_visible = &state.show_toast_bottom,
            .position = TOAST_BOTTOM,
            .on_close = close_toast
        }),

        Toast((ToastConfig){
            .message = "Task completed!",
            .is_visible = &state.show_toast_top,
            .position = TOAST_TOP,
            .on_close = close_toast
        }),

        Toast((ToastConfig){
            .message = "New message received",
            .is_visible = &state.show_toast_top_right,
            .position = TOAST_TOP_RIGHT,
            .on_close = close_toast
        }),

        Toast((ToastConfig){
            .message = "Download complete!",
            .is_visible = &state.show_toast_bottom_right,
            .position = TOAST_BOTTOM_RIGHT,
            .on_close = close_toast
        }),

        NULL
    );
}

int main(void) {
    tui_init();
    tui_set_root(app);
    tui_run();
    return 0;
}
