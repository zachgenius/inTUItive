#include "intuitive.h"
#include <stdio.h>
#include <time.h>

typedef struct {
    int cpu_usage;
    int memory_usage;
    int disk_usage;
    bool show_help;
    bool show_about;
} dashboard_state_t;

dashboard_state_t state = {42, 68, 73, false, false};

void toggle_help(void) {
    state.show_help = !state.show_help;
    tui_request_render();
}

void toggle_about(void) {
    state.show_about = !state.show_about;
    tui_request_render();
}

void close_modal(void) {
    state.show_help = false;
    state.show_about = false;
    tui_request_render();
}

component_t* stat_bar(const char* label, int percentage, color_t color) {
    char bar[32];
    int filled = (percentage * 20) / 100;

    for (int i = 0; i < 20; i++) {
        bar[i] = i < filled ? '=' : '-';
    }
    bar[20] = '\0';

    char percent_text[8];
    snprintf(percent_text, sizeof(percent_text), "%3d%%", percentage);

    return HStack(
        Text(label),
        Text(": ["),
        FgColor(Text(bar), color),
        Text("] "),
        Bold(Text(percent_text)),
        NULL
    );
}

component_t* app(void) {
    const char* recent_events[] = {
        "System started successfully",
        "Network connection established",
        "Background task completed",
        "Cache cleaned (2.3 GB freed)",
        "Update check completed",
    };

    component_t* main_ui = VStack(
        Bold(FgColor(Text("=== System Dashboard ==="), COLOR_BRIGHT_CYAN)),
        Text(""),

        FgColor(Text("System Resources:"), COLOR_BRIGHT_YELLOW),
        stat_bar("CPU    ", state.cpu_usage, COLOR_GREEN),
        stat_bar("Memory ", state.memory_usage, COLOR_YELLOW),
        stat_bar("Disk   ", state.disk_usage, COLOR_RED),
        Text(""),

        FgColor(Text("Recent Events:"), COLOR_BRIGHT_YELLOW),
        List(recent_events, 5),
        Text(""),

        HStack(
            Button("Help", toggle_help),
            Text("  "),
            Button("About", toggle_about),
            NULL
        ),
        Text(""),
        Text("Press 'q' to quit"),
        NULL
    );

    if (state.show_help) {
        component_t* help_content = VStack(
            Text("Dashboard Help"),
            Text(""),
            FgColor(Text("Navigation:"), COLOR_BRIGHT_GREEN),
            Text("  Tab - Switch between buttons"),
            Text("  Enter - Activate button"),
            Text("  q - Quit application"),
            Text(""),
            Text("Press any key to close..."),
            NULL
        );

        return VStack(
            main_ui,
            Modal(&state.show_help, "Help", help_content, close_modal),
            NULL
        );
    }

    if (state.show_about) {
        component_t* about_content = VStack(
            Bold(FgColor(Text("inTUItive Framework"), COLOR_BRIGHT_CYAN)),
            Text(""),
            Text("Version: 0.2.0 (Phase 2)"),
            Text(""),
            FgColor(Text("Features:"), COLOR_BRIGHT_GREEN),
            Text("  * Declarative UI"),
            Text("  * Colors & Styling"),
            Text("  * Interactive Components"),
            Text("  * Modal Dialogs"),
            Text("  * List Views"),
            Text(""),
            Underline(Text("github.com/yourusername/intuitive-tui")),
            NULL
        );

        return VStack(
            main_ui,
            Modal(&state.show_about, "About", about_content, close_modal),
            NULL
        );
    }

    return main_ui;
}

int main(void) {
    tui_init();
    tui_set_root(app);
    tui_run();
    return 0;
}
