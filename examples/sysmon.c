#include "intuitive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_PROCESSES 100
#define MAX_PROCESS_NAME 64

typedef struct {
    int pid;
    char name[MAX_PROCESS_NAME];
    float cpu;
    float mem;
    char user[32];
} process_info_t;

typedef struct {
    // System stats
    float cpu_usage;
    float mem_usage;
    long mem_total;
    long mem_used;
    int process_count_total;

    // Process list
    process_info_t processes[MAX_PROCESSES];
    int process_count;
    int selected_process;
    int process_scroll;

    // UI state
    time_t last_update;
    int update_interval;  // seconds
    char status_message[128];
} app_state_t;

static app_state_t state = {
    .cpu_usage = 0.0,
    .mem_usage = 0.0,
    .mem_total = 0,
    .mem_used = 0,
    .process_count_total = 0,
    .process_count = 0,
    .selected_process = 0,
    .process_scroll = 0,
    .last_update = 0,
    .update_interval = 1,
    .status_message = "Initializing..."
};

// Mutex to protect state access between threads
static pthread_mutex_t state_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t update_thread;
static bool thread_running = false;

/**
 * Initialize system info (run once at startup)
 */
static void init_system_info(void) {
    // Get total memory (doesn't change, so read once)
    FILE* fp = popen("sysctl hw.memsize | awk '{print $2}'", "r");
    if (fp) {
        if (fscanf(fp, "%ld", &state.mem_total) != 1) {
            state.mem_total = 0;
        }
        pclose(fp);
    }
}

/**
 * Read CPU usage from system (macOS)
 */
static float read_cpu_usage(void) {
    FILE* fp = popen("ps -A -o %cpu | awk '{s+=$1} END {print s}'", "r");
    if (!fp) return 0.0;

    float cpu = 0.0;
    if (fscanf(fp, "%f", &cpu) != 1) {
        cpu = 0.0;
    }
    pclose(fp);

    return cpu;
}

/**
 * Read memory usage from system (macOS)
 */
static void read_memory_usage(void) {
    // Get used memory (active + wired pages)
    FILE* fp = popen("vm_stat | grep -E 'Pages active|Pages wired' | awk '{print $3}' | tr -d '.'", "r");
    if (!fp) return;

    long active = 0, wired = 0;
    if (fscanf(fp, "%ld", &active) != 1) active = 0;
    if (fscanf(fp, "%ld", &wired) != 1) wired = 0;
    pclose(fp);

    // Pages are 4KB on macOS
    state.mem_used = (active + wired) * 4096;
    if (state.mem_total > 0) {
        state.mem_usage = (state.mem_used * 100.0) / state.mem_total;
    }
}

/**
 * Read process list from system
 */
static void read_processes(void) {
    // Get total process count
    FILE* fp = popen("ps aux | wc -l", "r");
    if (fp) {
        fscanf(fp, "%d", &state.process_count_total);
        pclose(fp);
        state.process_count_total--; // Subtract header line
    }

    // Get top processes by CPU
    fp = popen("ps aux | tail -n +2 | sort -rn -k 3 | head -50", "r");
    if (!fp) return;

    state.process_count = 0;
    char line[512];

    while (fgets(line, sizeof(line), fp) && state.process_count < MAX_PROCESSES) {
        process_info_t* proc = &state.processes[state.process_count];

        // Parse: USER PID %CPU %MEM ... COMMAND
        char command[256];
        if (sscanf(line, "%31s %d %f %f %*s %*s %*s %*s %*s %*s %255[^\n]",
                   proc->user, &proc->pid, &proc->cpu, &proc->mem, command) == 5) {

            // Extract just the command name (no args)
            char* space = strchr(command, ' ');
            if (space) *space = '\0';

            // Remove path, keep just filename
            char* slash = strrchr(command, '/');
            const char* name = slash ? slash + 1 : command;

            strncpy(proc->name, name, MAX_PROCESS_NAME - 1);
            proc->name[MAX_PROCESS_NAME - 1] = '\0';

            state.process_count++;
        }
    }

    pclose(fp);
}

/**
 * Background thread function to update system stats
 */
static void* update_thread_func(void* arg) {
    (void)arg;

    while (thread_running) {
        // Collect stats (this is slow, but happens in background)
        float cpu = read_cpu_usage();

        // Lock mutex before updating shared state
        pthread_mutex_lock(&state_mutex);

        state.cpu_usage = cpu;
        read_memory_usage();
        read_processes();

        time_t now = time(NULL);
        state.last_update = now;
        snprintf(state.status_message, sizeof(state.status_message),
                 "Updated at %s", ctime(&now));
        // Remove newline from ctime
        state.status_message[strlen(state.status_message) - 1] = '\0';

        pthread_mutex_unlock(&state_mutex);

        // Request UI re-render
        tui_request_render();

        // Sleep for update interval
        sleep(state.update_interval);
    }

    return NULL;
}

/**
 * Start background update thread
 */
static void start_update_thread(void) {
    thread_running = true;
    pthread_create(&update_thread, NULL, update_thread_func, NULL);
}

/**
 * Stop background update thread
 */
static void stop_update_thread(void) {
    thread_running = false;
    pthread_join(update_thread, NULL);
}

/**
 * Format bytes to human readable
 */
static void format_bytes(long bytes, char* buf, size_t size) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit = 0;
    double value = bytes;

    while (value >= 1024.0 && unit < 4) {
        value /= 1024.0;
        unit++;
    }

    snprintf(buf, size, "%.1f %s", value, units[unit]);
}

/**
 * Create a progress bar
 */
static component_t* progress_bar(float percentage, int width) {
    static char bar[256];
    int filled = (int)((percentage / 100.0) * width);
    if (filled > width) filled = width;

    int pos = 0;
    for (int i = 0; i < width; i++) {
        if (i < filled) {
            pos += snprintf(bar + pos, sizeof(bar) - pos, "█");
        } else {
            pos += snprintf(bar + pos, sizeof(bar) - pos, "░");
        }
    }

    color_t color = COLOR_BRIGHT_GREEN;
    if (percentage > 80) {
        color = COLOR_BRIGHT_RED;
    } else if (percentage > 60) {
        color = COLOR_BRIGHT_YELLOW;
    }

    return Text(bar, (TextConfig){
        .fg_color = color
    });
}

/**
 * Process list item selected
 */
static void on_process_select(int index) {
    state.selected_process = index;
}

static component_t* app(void) {
    int term_width, term_height;
    tui_get_terminal_size(&term_width, &term_height);

    // Copy state with mutex lock (minimize lock time)
    pthread_mutex_lock(&state_mutex);

    float cpu_usage = state.cpu_usage;
    float mem_usage = state.mem_usage;
    long mem_total = state.mem_total;
    long mem_used = state.mem_used;
    int process_count_total = state.process_count_total;
    int process_count = state.process_count;
    char status_message[128];
    strncpy(status_message, state.status_message, sizeof(status_message) - 1);
    status_message[sizeof(status_message) - 1] = '\0';

    // Copy process list
    process_info_t processes[MAX_PROCESSES];
    for (int i = 0; i < state.process_count; i++) {
        processes[i] = state.processes[i];
    }

    pthread_mutex_unlock(&state_mutex);

    // Format stats (outside mutex)
    char cpu_str[64];
    char mem_str[64];
    char mem_total_str[32];
    char mem_used_str[32];
    char process_count_str[64];

    snprintf(cpu_str, sizeof(cpu_str), "%.1f%%", cpu_usage);
    format_bytes(mem_total, mem_total_str, sizeof(mem_total_str));
    format_bytes(mem_used, mem_used_str, sizeof(mem_used_str));
    snprintf(mem_str, sizeof(mem_str), "%.1f%%", mem_usage);
    snprintf(process_count_str, sizeof(process_count_str),
             "Total Processes: %d (showing top %d by CPU)",
             process_count_total, process_count);

    // Create process list items
    const char* process_items[MAX_PROCESSES];
    static char process_lines[MAX_PROCESSES][128];

    for (int i = 0; i < process_count; i++) {
        process_info_t* proc = &processes[i];
        snprintf(process_lines[i], sizeof(process_lines[i]),
                 "%-6d  %5.1f%%  %5.1f%%  %-12s  %s",
                 proc->pid, proc->cpu, proc->mem, proc->user, proc->name);
        process_items[i] = process_lines[i];
    }

    int list_height = term_height - 18;
    if (list_height < 5) list_height = 5;

    return VStack(
        // Header
        AlignedVStack((StackConfig){
            .children = (component_t*[]){
                Text("=== SYSTEM MONITOR ===", (TextConfig){
                    .fg_color = COLOR_BRIGHT_CYAN,
                    .style = STYLE_BOLD
                }),
                NULL
            },
            .alignment = ALIGN_CENTER,
            .spacing = 0
        }),
        Text("", TEXT_DEFAULT),

        // Stats panel with padding and spacing
        Padded(
            VStack(
                // CPU stats
                HStack(
                    Text("CPU Usage:", (TextConfig){
                        .fg_color = COLOR_BRIGHT_YELLOW,
                        .style = STYLE_BOLD
                    }),
                    Text("  ", TEXT_DEFAULT),
                    Text(cpu_str, TEXT_DEFAULT),
                    NULL
                ),
                progress_bar(cpu_usage, 40),
                Text("", TEXT_DEFAULT),

                // Memory stats
                HStack(
                    Text("Memory:   ", (TextConfig){
                        .fg_color = COLOR_BRIGHT_YELLOW,
                        .style = STYLE_BOLD
                    }),
                    Text("  ", TEXT_DEFAULT),
                    Text(mem_str, TEXT_DEFAULT),
                    Text("  (", TEXT_DEFAULT),
                    Text(mem_used_str, TEXT_DEFAULT),
                    Text(" / ", TEXT_DEFAULT),
                    Text(mem_total_str, TEXT_DEFAULT),
                    Text(")", TEXT_DEFAULT),
                    NULL
                ),
                progress_bar(mem_usage, 40),
                NULL
            ),
            (PaddingConfig){ .top = 1, .bottom = 1, .left = 2, .right = 2 }
        ),

        // Process count
        Text(process_count_str, (TextConfig){
            .fg_color = COLOR_BRIGHT_BLACK
        }),
        Text("", TEXT_DEFAULT),

        // Process list header
        Text("Top Processes (by CPU usage):", (TextConfig){
            .style = STYLE_BOLD
        }),
        Text("PID     CPU    MEM    USER          NAME", (TextConfig){
            .fg_color = COLOR_BRIGHT_BLACK
        }),
        Text("────────────────────────────────────────────────────────────────────", (TextConfig){
            .fg_color = COLOR_BRIGHT_BLACK
        }),

        // Scrollable process list (List has built-in scrolling)
        List((ListConfig){
            .items = process_items,
            .count = process_count,
            .max_visible = list_height,
            .scroll_offset = &state.process_scroll,
            .selected_index = &state.selected_process,
            .on_select = on_process_select
        }),

        Text("", TEXT_DEFAULT),
        HStack(
            Text("Auto-updates every 1s", (TextConfig){ .fg_color = COLOR_BRIGHT_BLACK }),
            Text(" • ", TEXT_DEFAULT),
            Text("Tab to focus, ↑↓ or wheel to scroll", (TextConfig){ .fg_color = COLOR_BRIGHT_BLACK }),
            Text(" • ", TEXT_DEFAULT),
            Text("Click to select", (TextConfig){ .fg_color = COLOR_BRIGHT_BLACK }),
            Text(" • ", TEXT_DEFAULT),
            Text("'q' quits", (TextConfig){ .fg_color = COLOR_BRIGHT_BLACK }),
            NULL
        ),
        Text(status_message, (TextConfig){
            .fg_color = COLOR_BRIGHT_BLACK
        }),
        NULL
    );
}

int main(void) {
    // Initialize system info
    init_system_info();

    // Start background update thread
    start_update_thread();

    tui_init();
    tui_set_root(app);
    tui_run();

    // Stop background thread on exit
    stop_update_thread();

    return 0;
}
