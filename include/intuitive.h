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
 */
typedef enum {
    COLOR_DEFAULT = -1,
    COLOR_BLACK = 0,
    COLOR_RED = 1,
    COLOR_GREEN = 2,
    COLOR_YELLOW = 3,
    COLOR_BLUE = 4,
    COLOR_MAGENTA = 5,
    COLOR_CYAN = 6,
    COLOR_WHITE = 7,
    COLOR_BRIGHT_BLACK = 8,
    COLOR_BRIGHT_RED = 9,
    COLOR_BRIGHT_GREEN = 10,
    COLOR_BRIGHT_YELLOW = 11,
    COLOR_BRIGHT_BLUE = 12,
    COLOR_BRIGHT_MAGENTA = 13,
    COLOR_BRIGHT_CYAN = 14,
    COLOR_BRIGHT_WHITE = 15,
} color_t;

/**
 * Text style flags (can be combined with bitwise OR)
 */
typedef enum {
    STYLE_NONE = 0,
    STYLE_BOLD = 1 << 0,
    STYLE_UNDERLINE = 1 << 1,
} style_t;

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

/* ========== Components ========== */

/**
 * Create a Text component
 * Displays the given string
 */
component_t* Text(const char* str);

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
 * Create a Button component
 * Displays a clickable button that calls on_click when activated (Enter/Space)
 * Focusable via Tab key
 *
 * Example: Button("Click me", my_callback)
 */
component_t* Button(const char* label, void (*on_click)(void));

/**
 * Create an Input component
 * Text input field that edits the provided buffer
 * Focusable via Tab key
 * Supports typing, backspace, and arrow key navigation
 *
 * Example: Input(my_buffer, sizeof(my_buffer))
 */
component_t* Input(char* buffer, size_t size);

/**
 * Create a List component
 * Displays a scrollable list of items
 * Shows up to 10 items at a time, with automatic scrolling
 *
 * Example: List(items, 5) where items is const char*[]
 */
component_t* List(const char** items, int count);

/**
 * Create a Modal dialog component
 * Displays content in an overlay box with optional title
 * Only renders when *is_open is true
 * Call on_close() callback to handle dismissal (e.g., pressing ESC)
 *
 * Example: Modal(&show_modal, "Alert", Text("Hello!"), close_modal)
 */
component_t* Modal(bool* is_open, const char* title, component_t* content, void (*on_close)(void));

/**
 * Create a ScrollView component
 * Scrollable container that constrains content to max_height
 * Content taller than max_height can be scrolled with arrow keys
 * scroll_offset pointer stores the scroll position (persists across frames)
 *
 * Example:
 *   int scroll = 0;
 *   ScrollView(VStack(...long content..., NULL), &scroll, 10)
 */
component_t* ScrollView(component_t* content, int* scroll_offset, int max_height);

/**
 * Enable or disable scroll indicators for a ScrollView
 * Shows ▲/▼ arrows to indicate scrollable content
 * Returns the same component for chaining
 *
 * Example: ScrollIndicators(ScrollView(...), false)
 */
component_t* ScrollIndicators(component_t* comp, bool enabled);

/* ========== Style Modifiers ========== */

/**
 * Set both foreground and background colors for a component
 * Returns the same component for chaining
 *
 * Example: Color(Text("Error!"), COLOR_RED, COLOR_DEFAULT)
 */
component_t* Color(component_t* comp, color_t fg, color_t bg);

/**
 * Set foreground color for a component
 * Returns the same component for chaining
 *
 * Example: FgColor(Text("Success!"), COLOR_GREEN)
 */
component_t* FgColor(component_t* comp, color_t fg);

/**
 * Set background color for a component
 * Returns the same component for chaining
 *
 * Example: BgColor(Text("Highlighted"), COLOR_YELLOW)
 */
component_t* BgColor(component_t* comp, color_t bg);

/**
 * Make a component's text bold
 * Returns the same component for chaining
 *
 * Example: Bold(Text("Important!"))
 */
component_t* Bold(component_t* comp);

/**
 * Underline a component's text
 * Returns the same component for chaining
 *
 * Example: Underline(Text("Link"))
 */
component_t* Underline(component_t* comp);
