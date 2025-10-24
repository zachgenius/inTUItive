#include "intuitive.h"
#include <stdio.h>

typedef struct {
    float progress;
} app_state_t;

static app_state_t state = {
    .progress = 0.0f
};

static void increment_progress(void) {
    state.progress += 5.0f;
    if (state.progress > 100.0f) {
        state.progress = 0.0f;
    }
    tui_request_render();
}

static void decrement_progress(void) {
    state.progress -= 5.0f;
    if (state.progress < 0.0f) {
        state.progress = 100.0f;
    }
    tui_request_render();
}

static component_t* app(void) {
    return VStack(
        Text("=== SPINNER DEMO ===", &(TextConfig){
            .fg_color = COLOR_BRIGHT_CYAN,
            .style = STYLE_BOLD
        }),
        Text("", NULL),

        Text("Basic Spinners:", NULL),
        Text("", NULL),

        HStack(
            Text("Braille:  ", NULL),
            Spinner((SpinnerConfig){
                .style = SPINNER_BRAILLE,
                .speed = 80,
                .text = NULL
            }),
            NULL
        ),

        HStack(
            Text("Classic:  ", NULL),
            Spinner((SpinnerConfig){
                .style = SPINNER_CLASSIC,
                .speed = 100,
                .text = NULL
            }),
            NULL
        ),

        HStack(
            Text("Dots:     ", NULL),
            Spinner((SpinnerConfig){
                .style = SPINNER_DOTS,
                .speed = 80,
                .text = NULL
            }),
            NULL
        ),

        HStack(
            Text("Box:      ", NULL),
            Spinner((SpinnerConfig){
                .style = SPINNER_BOX,
                .speed = 120,
                .text = NULL
            }),
            NULL
        ),

        HStack(
            Text("Arrow:    ", NULL),
            Spinner((SpinnerConfig){
                .style = SPINNER_ARROW,
                .speed = 100,
                .text = NULL
            }),
            NULL
        ),

        Text("", NULL),
        Text("Spinners with Text:", NULL),
        Text("", NULL),

        Spinner((SpinnerConfig){
            .style = SPINNER_BRAILLE,
            .speed = 80,
            .text = "Loading data..."
        }),

        Spinner((SpinnerConfig){
            .style = SPINNER_DOTS,
            .speed = 80,
            .text = "Processing..."
        }),

        Text("", NULL),
        Text("Spinner with Progress:", NULL),
        Text("", NULL),

        Spinner((SpinnerConfig){
            .style = SPINNER_BRAILLE,
            .speed = 80,
            .text = "Downloading",
            .progress = &state.progress
        }),

        Text("", NULL),
        HStack(
            Button("-5%", decrement_progress),
            Text("  ", NULL),
            Button("+5%", increment_progress),
            NULL
        ),

        Text("", NULL),
        Text("Tab to focus buttons, +/- to change progress, q to quit", &(TextConfig){
            .fg_color = COLOR_BRIGHT_BLACK
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
