#pragma once

#include <stddef.h>
#include <stdbool.h>

/**
 * inTUItive - A declarative TUI framework for C
 *
 * This is the only header you need to include to use inTUItive.
 *
 * Example usage:
 *
 *   #include "intuitive.h"
 *
 *   component_t* app(void) {
 *       return VStack(
 *           Text("Hello, inTUItive!"),
 *           Text("Welcome to declarative TUI"),
 *           NULL
 *       );
 *   }
 *
 *   int main() {
 *       tui_init();
 *       tui_set_root(app);
 *       tui_run();
 *       return 0;
 *   }
 */

/* Opaque component type - users don't need to see the internals */
typedef struct component_t component_t;

/* ========== Colors and Styles ========== */

/**
 * Terminal colors (ANSI 16-color palette)
 * COLOR_DEFAULT = 0 for easy zero-initialization
 */
typedef enum {
    COLOR_DEFAULT = 0,
    COLOR_BLACK = 1,
    COLOR_RED = 2,
    COLOR_GREEN = 3,
    COLOR_YELLOW = 4,
    COLOR_BLUE = 5,
    COLOR_MAGENTA = 6,
    COLOR_CYAN = 7,
    COLOR_WHITE = 8,
    COLOR_BRIGHT_BLACK = 9,
    COLOR_BRIGHT_RED = 10,
    COLOR_BRIGHT_GREEN = 11,
    COLOR_BRIGHT_YELLOW = 12,
    COLOR_BRIGHT_BLUE = 13,
    COLOR_BRIGHT_MAGENTA = 14,
    COLOR_BRIGHT_CYAN = 15,
    COLOR_BRIGHT_WHITE = 16,
} color_t;

/**
 * Text style flags (can be combined with bitwise OR)
 */
typedef enum {
    STYLE_NONE = 0,
    STYLE_BOLD = 1 << 0,
    STYLE_UNDERLINE = 1 << 1,
} style_t;

/**
 * Alignment options for layout containers
 */
typedef enum {
    ALIGN_START = 0,    // Left for HStack, Top for VStack
    ALIGN_CENTER = 1,   // Center alignment
    ALIGN_END = 2,      // Right for HStack, Bottom for VStack
} alignment_t;

/**
 * Padding configuration
 */
typedef struct {
    int top;
    int bottom;
    int left;
    int right;
} PaddingConfig;

/* ========== TUI Core API ========== */

/**
 * Initialize the TUI framework
 * Must be called before any other TUI functions
 */
void tui_init(void);

/**
 * Set the root component function
 * The function will be called to build the UI tree
 */
void tui_set_root(component_t* (*root_fn)(void));

/**
 * Run the TUI event loop
 * This will render the UI and handle events until quit (press 'q')
 */
void tui_run(void);

/**
 * Request a re-render on the next event loop iteration
 * Call this after modifying application state
 */
void tui_request_render(void);

/**
 * Get the current terminal dimensions
 * Returns true if successful, false otherwise
 * Width and height are measured in character cells
 */
bool tui_get_terminal_size(int* width, int* height);

/* ========== Components ========== */

/**
 * Text configuration (styling)
 * Unspecified fields default to: fg_color=COLOR_DEFAULT, bg_color=COLOR_DEFAULT, style=STYLE_NONE
 */
typedef struct {
    color_t fg_color;       // Foreground color (default: COLOR_DEFAULT)
    color_t bg_color;       // Background color (default: COLOR_DEFAULT)
    style_t style;          // Text style flags (default: STYLE_NONE, can combine with |)
} TextConfig;

/**
 * Default text configuration (no styling)
 * Use this for plain text without any colors or styles
 */
#define TEXT_DEFAULT ((TextConfig){ .fg_color = COLOR_DEFAULT, .bg_color = COLOR_DEFAULT, .style = STYLE_NONE })

/**
 * Create a Text component
 * First parameter is the text content (required)
 * Second parameter is the styling config (use TEXT_DEFAULT for no styling)
 *
 * Example (simple):
 *   Text("Hello", TEXT_DEFAULT)
 *
 * Example (colored):
 *   Text("Error", (TextConfig){ .fg_color = COLOR_RED })
 *
 * Example (fully styled):
 *   Text("Important!", (TextConfig){
 *       .fg_color = COLOR_BRIGHT_RED,
 *       .bg_color = COLOR_BLACK,
 *       .style = STYLE_BOLD | STYLE_UNDERLINE
 *   })
 */
component_t* Text(const char* content, TextConfig config);

/**
 * Create a VStack (vertical stack) component
 * Arranges children vertically
 * Arguments must be NULL-terminated
 *
 * Example: VStack(Text("A"), Text("B"), Text("C"), NULL)
 */
component_t* VStack(component_t* first, ...);

/**
 * Create an HStack (horizontal stack) component
 * Arranges children horizontally
 * Arguments must be NULL-terminated
 *
 * Example: HStack(Text("A"), Text("B"), Text("C"), NULL)
 */
component_t* HStack(component_t* first, ...);

/**
 * Create a VStack from a NULL-terminated array of components
 * Useful when the number of children is dynamic
 *
 * Example: VStackArray(items) where items is component_t*[]
 */
component_t* VStackArray(component_t** children);

/**
 * StackConfig for advanced stack layout control
 */
typedef struct {
    component_t** children;  // NULL-terminated array of children
    alignment_t alignment;   // Horizontal alignment for VStack, vertical for HStack
    int spacing;             // Space between children (default: 0)
} StackConfig;

/**
 * Create a VStack with alignment and spacing options
 *
 * Example: AlignedVStack((StackConfig){
 *     .children = (component_t*[]){ Text("A"), Text("B"), NULL },
 *     .alignment = ALIGN_CENTER,
 *     .spacing = 1
 * })
 */
component_t* AlignedVStack(StackConfig config);

/**
 * Create an HStack with alignment and spacing options
 *
 * Example: AlignedHStack((StackConfig){
 *     .children = (component_t*[]){ Text("A"), Text("B"), NULL },
 *     .alignment = ALIGN_CENTER,
 *     .spacing = 1
 * })
 */
component_t* AlignedHStack(StackConfig config);

/**
 * Add padding around a component
 * Padding adds space inside the component's bounds
 *
 * Example: Padded(Text("Hello"), (PaddingConfig){ .all = 2 })
 * Example: Padded(Text("Hello"), (PaddingConfig){ .top = 1, .left = 2, .right = 2, .bottom = 1 })
 */
component_t* Padded(component_t* child, PaddingConfig padding);

/**
 * Create a flexible spacer component
 * In HStack: pushes siblings to left/right edges
 * In VStack: pushes siblings to top/bottom edges
 *
 * Example: HStack(Text("Left"), Spacer(), Text("Right"), NULL)
 */
component_t* Spacer(void);

/**
 * Create a Button component
 * Displays a clickable button that calls on_click when activated (Enter/Space)
 * Focusable via Tab key
 *
 * Example: Button("Click me", my_callback)
 */
component_t* Button(const char* label, void (*on_click)(void));

/**
 * Input configuration
 */
typedef struct {
    char* buffer;       // Text buffer to edit
    size_t size;        // Size of the buffer
} InputConfig;

/**
 * Create an Input component
 * Text input field that edits the provided buffer
 * Focusable via Tab key
 * Supports typing, backspace, and arrow key navigation
 *
 * Example: Input((InputConfig){ .buffer = my_buffer, .size = sizeof(my_buffer) })
 */
component_t* Input(InputConfig config);

/**
 * List configuration
 */
typedef struct {
    const char** items;          // Array of item strings
    int count;                   // Number of items
    int max_visible;             // Maximum visible items (default: 10)
    int* scroll_offset;          // Pointer to scroll position (external state, optional)
    int* selected_index;         // Pointer to selected index (external state, optional)
    void (*on_select)(int index); // Callback when Enter pressed on item (optional)
} ListConfig;

/**
 * Create a List component
 * Displays a scrollable list of items
 * Shows up to max_visible items at a time, with automatic scrolling
 *
 * Example: List((ListConfig){ .items = items, .count = 5, .max_visible = 10 })
 */
component_t* List(ListConfig config);

/**
 * Modal configuration
 */
typedef struct {
    bool* is_open;           // Pointer to open/close state
    const char* title;       // Optional title (can be NULL)
    component_t* content;    // Content to display
    void (*on_close)(void);  // Callback when modal is closed
} ModalConfig;

/**
 * Create a Modal dialog component
 * Displays content in an overlay box with optional title
 * Only renders when *is_open is true
 * Call on_close() callback to handle dismissal (e.g., pressing ESC)
 *
 * Example: Modal((ModalConfig){
 *     .is_open = &show_modal,
 *     .title = "Alert",
 *     .content = Text("Hello!"),
 *     .on_close = close_modal
 * })
 */
component_t* Modal(ModalConfig config);

/**
 * ScrollView configuration
 */
typedef struct {
    int max_height;          // Maximum visible height of the viewport
    bool show_indicators;    // Show scroll bar (default: true)
    const char* thumb_focused;    // Character for thumb when focused (default: "█")
    const char* thumb_unfocused;  // Character for thumb when not focused (default: "▓")
    const char* track_char;       // Character for track (default: "│")
    bool show_arrows;        // Show ▲/▼ arrows (default: true)
} ScrollConfig;

/**
 * Create a ScrollView component
 * Scrollable container that constrains content to max_height
 * Content taller than max_height can be scrolled with arrow keys
 * scroll_offset pointer stores the scroll position (persists across frames)
 *
 * Example (with defaults):
 *   int scroll = 0;
 *   ScrollView(VStack(...long content..., NULL), &scroll, (ScrollConfig){
 *       .max_height = 10,
 *       .show_indicators = true,
 *       .show_arrows = true
 *   })
 *
 * Example (custom appearance):
 *   ScrollView(content, &scroll, (ScrollConfig){
 *       .max_height = 20,
 *       .show_indicators = true,
 *       .thumb_focused = "■",      // Solid square when focused
 *       .thumb_unfocused = "□",    // Hollow square when not focused
 *       .track_char = "┆",         // Dotted line for track
 *       .show_arrows = false       // No arrows
 *   })
 */
component_t* ScrollView(component_t* content, int* scroll_offset, ScrollConfig config);

/**
 * Table configuration
 */
typedef struct {
    const char** headers;    // Array of header strings
    const char*** rows;      // 2D array of cell strings [row][col]
    int column_count;        // Number of columns
    int row_count;           // Number of rows
    bool show_borders;       // Show borders around cells (default: true)
} TableConfig;

/**
 * Create a Table component
 * Displays structured data in rows and columns with headers
 *
 * Example:
 *   const char* headers[] = {"Name", "Age", "City"};
 *   const char* row1[] = {"Alice", "30", "NYC"};
 *   const char* row2[] = {"Bob", "25", "LA"};
 *   const char** rows[] = {row1, row2};
 *   Table((TableConfig){
 *       .headers = headers,
 *       .rows = rows,
 *       .column_count = 3,
 *       .row_count = 2,
 *       .show_borders = true
 *   })
 */
component_t* Table(TableConfig config);

/**
 * Spinner style types
 */
typedef enum {
    SPINNER_BRAILLE,  /* Braille dots spinner */
    SPINNER_CLASSIC,  /* Classic | / - \ spinner */
    SPINNER_DOTS,     /* Braille dots variant */
    SPINNER_BOX,      /* Box corners spinner */
    SPINNER_ARROW,    /* Arrow spinner */
} spinner_style_t;

/**
 * Spinner configuration
 */
typedef struct {
    spinner_style_t style;  // Spinner animation style
    int speed;              // Milliseconds per frame (default: 100)
    const char* text;       // Optional text label (can be NULL)
    float* progress;        // Optional progress percentage 0-100 (can be NULL)
} SpinnerConfig;

/**
 * Create a Spinner component
 * Displays an animated loading indicator
 *
 * Example (basic):
 *   Spinner((SpinnerConfig){
 *       .style = SPINNER_BRAILLE,
 *       .speed = 100,
 *       .text = "Loading..."
 *   })
 *
 * Example (with progress):
 *   float progress = 45.0f;
 *   Spinner((SpinnerConfig){
 *       .style = SPINNER_DOTS,
 *       .speed = 80,
 *       .text = "Processing...",
 *       .progress = &progress
 *   })
 */
component_t* Spinner(SpinnerConfig config);

/* ========== Convenience Macros ========== */

/**
 * Common text style shortcuts
 */
#define ErrorText(text) Text(text, (TextConfig){ .fg_color = COLOR_BRIGHT_RED, .style = STYLE_BOLD })
#define SuccessText(text) Text(text, (TextConfig){ .fg_color = COLOR_BRIGHT_GREEN, .style = STYLE_BOLD })
#define WarningText(text) Text(text, (TextConfig){ .fg_color = COLOR_BRIGHT_YELLOW, .style = STYLE_BOLD })
#define InfoText(text) Text(text, (TextConfig){ .fg_color = COLOR_BRIGHT_CYAN })
#define MutedText(text) Text(text, (TextConfig){ .fg_color = COLOR_BRIGHT_BLACK })

/**
 * Config struct initializers with common defaults
 */
#define INPUT(buffer, size) (InputConfig){ .buffer = buffer, .size = size }
#define LIST(items, count) (ListConfig){ .items = items, .count = count, .max_visible = 10, .scroll_offset = NULL, .selected_index = NULL, .on_select = NULL }
#define LIST_SELECTABLE(items, count, scroll, selected, callback) (ListConfig){ .items = items, .count = count, .max_visible = 10, .scroll_offset = scroll, .selected_index = selected, .on_select = callback }
#define MODAL(is_open, title, content, on_close) (ModalConfig){ .is_open = is_open, .title = title, .content = content, .on_close = on_close }
#define SCROLLVIEW(height, show_indicators) (ScrollConfig){ .max_height = height, .show_indicators = show_indicators, .thumb_focused = NULL, .thumb_unfocused = NULL, .track_char = NULL, .show_arrows = true }
#define TABLE(headers, rows, cols, rows_count, borders) (TableConfig){ .headers = headers, .rows = rows, .column_count = cols, .row_count = rows_count, .show_borders = borders }
