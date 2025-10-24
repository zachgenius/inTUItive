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
        Text(label, NULL),
        Text(": [", NULL),
        Text(bar, &(TextConfig){ .fg_color = color }),
        Text("] ", NULL),
        Text(percent_text, &(TextConfig){ .style = STYLE_BOLD }),
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
        Text("=== System Dashboard ===", &(TextConfig){
            .fg_color = COLOR_BRIGHT_CYAN,
            .style = STYLE_BOLD
        }),
        Text("", NULL),

        Text("System Resources:", &(TextConfig){ .fg_color = COLOR_BRIGHT_YELLOW }),
        stat_bar("CPU    ", state.cpu_usage, COLOR_GREEN),
        stat_bar("Memory ", state.memory_usage, COLOR_YELLOW),
        stat_bar("Disk   ", state.disk_usage, COLOR_RED),
        Text("", NULL),

        Text("Recent Events:", &(TextConfig){ .fg_color = COLOR_BRIGHT_YELLOW }),
        List((ListConfig){
            .items = recent_events,
            .count = 5,
            .max_visible = 10
        }),
        Text("", NULL),

        HStack(
            Button("Help", toggle_help),
            Text("  ", NULL),
            Button("About", toggle_about),
            NULL
        ),
        Text("", NULL),
        Text("Press 'q' to quit", NULL),
        NULL
    );

    if (state.show_help) {
        component_t* help_content = VStack(
            Text("Dashboard Help", NULL),
            Text("", NULL),
            Text("Navigation:", &(TextConfig){ .fg_color = COLOR_BRIGHT_GREEN }),
            Text("  Tab - Switch between buttons", NULL),
            Text("  Enter - Activate button", NULL),
            Text("  Esc - Close modal", NULL),
            Text("  q - Quit application", NULL),
            Text("", NULL),
            Text("Press any key to close...", NULL),
            NULL
        );

        return VStack(
            main_ui,
            Modal((ModalConfig){
                .is_open = &state.show_help,
                .title = "Help",
                .content = help_content,
                .on_close = close_modal
            }),
            NULL
        );
    }

    if (state.show_about) {
        component_t* about_content = VStack(
            Text("inTUItive Framework", &(TextConfig){
                .fg_color = COLOR_BRIGHT_CYAN,
                .style = STYLE_BOLD
            }),
            Text("", NULL),
            Text("Version: 0.2.0 (Phase 2)", NULL),
            Text("", NULL),
            Text("Features:", &(TextConfig){ .fg_color = COLOR_BRIGHT_GREEN }),
            Text("  * Declarative UI", NULL),
            Text("  * Colors & Styling", NULL),
            Text("  * Interactive Components", NULL),
            Text("  * Modal Dialogs", NULL),
            Text("  * List Views", NULL),
            Text("", NULL),
            Text("github.com/yourusername/intuitive-tui", &(TextConfig){
                .style = STYLE_UNDERLINE
            }),
            Text("", NULL),
            Text("Press any key to close...", NULL),
            NULL
        );

        return VStack(
            main_ui,
            Modal((ModalConfig){
                .is_open = &state.show_about,
                .title = "About",
                .content = about_content,
                .on_close = close_modal
            }),
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
