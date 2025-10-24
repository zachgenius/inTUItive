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
        Text(label, TEXT_DEFAULT),
        Text(": [", TEXT_DEFAULT),
        Text(bar, (TextConfig){ .fg_color = color }),
        Text("] ", TEXT_DEFAULT),
        Text(percent_text, (TextConfig){ .style = STYLE_BOLD }),
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
        Text("=== System Dashboard ===", (TextConfig){
            .fg_color = COLOR_BRIGHT_CYAN,
            .style = STYLE_BOLD
        }),
        Text("", TEXT_DEFAULT),

        Text("System Resources:", (TextConfig){ .fg_color = COLOR_BRIGHT_YELLOW }),
        stat_bar("CPU    ", state.cpu_usage, COLOR_GREEN),
        stat_bar("Memory ", state.memory_usage, COLOR_YELLOW),
        stat_bar("Disk   ", state.disk_usage, COLOR_RED),
        Text("", TEXT_DEFAULT),

        Text("Recent Events:", (TextConfig){ .fg_color = COLOR_BRIGHT_YELLOW }),
        List((ListConfig){
            .items = recent_events,
            .count = 5,
            .max_visible = 10
        }),
        Text("", TEXT_DEFAULT),

        HStack(
            Button("Help", toggle_help),
            Text("  ", TEXT_DEFAULT),
            Button("About", toggle_about),
            NULL
        ),
        Text("", TEXT_DEFAULT),
        Text("Press 'q' to quit", TEXT_DEFAULT),
        NULL
    );

    if (state.show_help) {
        component_t* help_content = VStack(
            Text("Dashboard Help", TEXT_DEFAULT),
            Text("", TEXT_DEFAULT),
            Text("Navigation:", (TextConfig){ .fg_color = COLOR_BRIGHT_GREEN }),
            Text("  Tab - Switch between buttons", TEXT_DEFAULT),
            Text("  Enter - Activate button", TEXT_DEFAULT),
            Text("  Esc - Close modal", TEXT_DEFAULT),
            Text("  q - Quit application", TEXT_DEFAULT),
            Text("", TEXT_DEFAULT),
            Text("Press any key to close...", TEXT_DEFAULT),
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
            Text("inTUItive Framework", (TextConfig){
                .fg_color = COLOR_BRIGHT_CYAN,
                .style = STYLE_BOLD
            }),
            Text("", TEXT_DEFAULT),
            Text("Version: 0.2.0 (Phase 2)", TEXT_DEFAULT),
            Text("", TEXT_DEFAULT),
            Text("Features:", (TextConfig){ .fg_color = COLOR_BRIGHT_GREEN }),
            Text("  * Declarative UI", TEXT_DEFAULT),
            Text("  * Colors & Styling", TEXT_DEFAULT),
            Text("  * Interactive Components", TEXT_DEFAULT),
            Text("  * Modal Dialogs", TEXT_DEFAULT),
            Text("  * List Views", TEXT_DEFAULT),
            Text("", TEXT_DEFAULT),
            Text("github.com/yourusername/intuitive-tui", (TextConfig){
                .style = STYLE_UNDERLINE
            }),
            Text("", TEXT_DEFAULT),
            Text("Press any key to close...", TEXT_DEFAULT),
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
