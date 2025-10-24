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
        Text("=== SPINNER DEMO ===", (TextConfig){
            .fg_color = COLOR_BRIGHT_CYAN,
            .style = STYLE_BOLD
        }),
        Text("", TEXT_DEFAULT),

        Text("Basic Spinners:", TEXT_DEFAULT),
        Text("", TEXT_DEFAULT),

        HStack(
            Text("Braille:  ", TEXT_DEFAULT),
            Spinner((SpinnerConfig){
                .style = SPINNER_BRAILLE,
                .speed = 80,
                .text = NULL
            }),
            NULL
        ),

        HStack(
            Text("Classic:  ", TEXT_DEFAULT),
            Spinner((SpinnerConfig){
                .style = SPINNER_CLASSIC,
                .speed = 100,
                .text = NULL
            }),
            NULL
        ),

        HStack(
            Text("Dots:     ", TEXT_DEFAULT),
            Spinner((SpinnerConfig){
                .style = SPINNER_DOTS,
                .speed = 80,
                .text = NULL
            }),
            NULL
        ),

        HStack(
            Text("Box:      ", TEXT_DEFAULT),
            Spinner((SpinnerConfig){
                .style = SPINNER_BOX,
                .speed = 120,
                .text = NULL
            }),
            NULL
        ),

        HStack(
            Text("Arrow:    ", TEXT_DEFAULT),
            Spinner((SpinnerConfig){
                .style = SPINNER_ARROW,
                .speed = 100,
                .text = NULL
            }),
            NULL
        ),

        Text("", TEXT_DEFAULT),
        Text("Spinners with Text:", TEXT_DEFAULT),
        Text("", TEXT_DEFAULT),

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

        Text("", TEXT_DEFAULT),
        Text("Spinner with Progress:", TEXT_DEFAULT),
        Text("", TEXT_DEFAULT),

        Spinner((SpinnerConfig){
            .style = SPINNER_BRAILLE,
            .speed = 80,
            .text = "Downloading",
            .progress = &state.progress
        }),

        Text("", TEXT_DEFAULT),
        HStack(
            Button("-5%", decrement_progress),
            Text("  ", TEXT_DEFAULT),
            Button("+5%", increment_progress),
            NULL
        ),

        Text("", TEXT_DEFAULT),
        Text("Tab to focus buttons, +/- to change progress, q to quit", (TextConfig){
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
