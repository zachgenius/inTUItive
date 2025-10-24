#define _POSIX_C_SOURCE 200809L

#include "intuitive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <limits.h>

#define MAX_PATH 1024
#define MAX_FILES 1000

typedef struct {
    char current_path[MAX_PATH];
    char* file_list[MAX_FILES];
    int file_count;
    int scroll_pos;
    int selected_index;
    bool show_error;
    char error_msg[256];
} fm_state_t;

fm_state_t state = {0};

void close_error(void) {
    state.show_error = false;
    tui_request_render();
}

void show_error(const char* msg) {
    strncpy(state.error_msg, msg, sizeof(state.error_msg) - 1);
    state.show_error = true;
    tui_request_render();
}

void free_file_list(void) {
    for (int i = 0; i < state.file_count; i++) {
        free(state.file_list[i]);
        state.file_list[i] = NULL;
    }
    state.file_count = 0;
}

int compare_files(const void* a, const void* b) {
    const char* file_a = *(const char**)a;
    const char* file_b = *(const char**)b;

    // ".." always first
    if (strcmp(file_a, "📁 ..") == 0) return -1;
    if (strcmp(file_b, "📁 ..") == 0) return 1;

    // Directories before files
    bool a_is_dir = strncmp(file_a, "📁", 4) == 0;
    bool b_is_dir = strncmp(file_b, "📁", 4) == 0;

    if (a_is_dir && !b_is_dir) return -1;
    if (!a_is_dir && b_is_dir) return 1;

    return strcmp(file_a, file_b);
}

void load_directory(const char* path) {
    free_file_list();
    state.scroll_pos = 0;
    state.selected_index = 0;

    DIR* dir = opendir(path);
    if (!dir) {
        show_error("Failed to open directory");
        return;
    }

    // Add parent directory entry if not at root
    if (strcmp(path, "/") != 0) {
        state.file_list[state.file_count++] = strdup("📁 ..");
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL && state.file_count < MAX_FILES) {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Build full path for stat
        char full_path[MAX_PATH];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) == 0) {
            char display_name[512];
            if (S_ISDIR(st.st_mode)) {
                snprintf(display_name, sizeof(display_name), "📁 %s", entry->d_name);
            } else if (st.st_size > 1024 * 1024) {
                snprintf(display_name, sizeof(display_name), "📄 %s (%.1fM)",
                         entry->d_name, st.st_size / (1024.0 * 1024.0));
            } else if (st.st_size > 1024) {
                snprintf(display_name, sizeof(display_name), "📄 %s (%.1fK)",
                         entry->d_name, st.st_size / 1024.0);
            } else {
                snprintf(display_name, sizeof(display_name), "📄 %s (%ldB)",
                         entry->d_name, (long)st.st_size);
            }
            state.file_list[state.file_count++] = strdup(display_name);
        }
    }

    closedir(dir);

    // Sort: directories first, then alphabetically
    if (state.file_count > 0) {
        qsort(state.file_list, state.file_count, sizeof(char*), compare_files);
    }
}

void navigate_to(const char* path) {
    char resolved[MAX_PATH];
    if (realpath(path, resolved) == NULL) {
        show_error("Invalid path");
        return;
    }

    strncpy(state.current_path, resolved, sizeof(state.current_path) - 1);
    load_directory(state.current_path);
    tui_request_render();
}

void navigate_up(void) {
    char parent[MAX_PATH];
    snprintf(parent, sizeof(parent), "%s/..", state.current_path);
    navigate_to(parent);
}

void navigate_into(int index) {
    if (index < 0 || index >= state.file_count) {
        return;
    }

    const char* selected = state.file_list[index];

    // Check if it's ".."
    if (strcmp(selected, "📁 ..") == 0) {
        navigate_up();
        return;
    }

    // Check if it's a directory (starts with 📁)
    if (strncmp(selected, "📁", 4) != 0) {
        show_error("Not a directory - file preview not implemented yet");
        return;
    }

    // Extract directory name (skip "📁 " prefix)
    const char* dir_name = selected + 5;

    char new_path[MAX_PATH];
    snprintf(new_path, sizeof(new_path), "%s/%s", state.current_path, dir_name);
    navigate_to(new_path);
}

component_t* app(void) {
    component_t* items[MAX_FILES + 20];
    int idx = 0;

    // Header
    items[idx++] = Bold(FgColor(Text("=== File Manager ==="), COLOR_BRIGHT_CYAN));
    items[idx++] = Text("");

    // Current path
    char path_label[MAX_PATH + 20];
    snprintf(path_label, sizeof(path_label), "Path: %s", state.current_path);
    items[idx++] = FgColor(Text(path_label), COLOR_BRIGHT_YELLOW);
    items[idx++] = Text("");

    // File count
    char count_label[64];
    snprintf(count_label, sizeof(count_label), "Items: %d", state.file_count);
    items[idx++] = Text(count_label);
    items[idx++] = Text("");

    // File list with ScrollView for long directories
    if (state.file_count > 0) {
        items[idx++] = Text("Files and Directories:");

        // Create scrollable list of files
        component_t* file_items[MAX_FILES + 1];
        for (int i = 0; i < state.file_count && i < MAX_FILES; i++) {
            file_items[i] = Text(state.file_list[i]);
        }
        file_items[state.file_count] = NULL;

        component_t* file_list = VStackArray(file_items);
        items[idx++] = ScrollView(file_list, &state.scroll_pos, (ScrollConfig){
            .max_height = 15,
            .show_indicators = true
        });
    } else {
        items[idx++] = FgColor(Text("(empty directory)"), COLOR_BRIGHT_BLACK);
    }

    items[idx++] = Text("");
    items[idx++] = Text("");

    // Controls
    items[idx++] = FgColor(Text("Controls:"), COLOR_BRIGHT_GREEN);
    items[idx++] = Text("  Tab - Focus file list");
    items[idx++] = Text("  Up/Down - Scroll through files");
    items[idx++] = Text("  q - Quit");
    items[idx++] = Text("");
    items[idx++] = FgColor(Text("Note: Navigation not yet implemented"), COLOR_BRIGHT_BLACK);

    items[idx] = NULL;

    component_t* main_ui = VStackArray(items);

    // Show error modal if needed
    if (state.show_error) {
        return VStack(
            main_ui,
            Modal((ModalConfig){
                .is_open = &state.show_error,
                .title = "Error",
                .content = FgColor(Text(state.error_msg), COLOR_BRIGHT_RED),
                .on_close = close_error
            }),
            NULL
        );
    }

    return main_ui;
}

int main(void) {
    // Initialize with current directory
    char cwd[MAX_PATH];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        strncpy(state.current_path, cwd, sizeof(state.current_path) - 1);
    } else {
        strncpy(state.current_path, "/", sizeof(state.current_path) - 1);
    }

    load_directory(state.current_path);

    tui_init();
    tui_set_root(app);
    tui_run();

    // Cleanup
    free_file_list();

    return 0;
}
