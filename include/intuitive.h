#pragma once

#include <stddef.h>

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
