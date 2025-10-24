#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "../include/intuitive.h"

/**
 * Component types
 */
typedef enum {
    COMPONENT_TEXT,
    COMPONENT_VSTACK,
    COMPONENT_HSTACK,
    COMPONENT_BUTTON,
    COMPONENT_INPUT,
    COMPONENT_LIST,
    COMPONENT_MODAL,
    COMPONENT_SCROLLVIEW,
} component_type_t;

/**
 * Component structure - base type for all UI elements
 */
struct component_t {
    component_type_t type;

    // Component-specific data (owned by component)
    void* data;

    // Children (for container components)
    struct component_t** children;
    int child_count;
    int child_capacity;  // For dynamic array growth

    // Layout information (computed during layout pass)
    int x, y;
    int width, height;

    // Focus information
    bool focusable;
    bool focused;
    int focus_index;

    // Style information
    color_t fg_color;
    color_t bg_color;
    style_t style;
};

/**
 * Text component data
 */
typedef struct {
    char* content;
} text_data_t;

/**
 * Button component data
 */
typedef struct {
    char* label;
    void (*on_click)(void);
} button_data_t;

/**
 * Input component data
 */
typedef struct {
    char* buffer;
    size_t buffer_size;
    size_t cursor_pos;
    size_t scroll_offset;
} input_data_t;

/**
 * List component data
 */
typedef struct {
    char** items;
    int item_count;
    int scroll_offset;
    int max_visible_items;
} list_data_t;

/**
 * Modal component data
 */
typedef struct {
    bool* is_open;
    char* title;
    struct component_t* content;
    void (*on_close)(void);
} modal_data_t;

/**
 * ScrollView component data
 */
typedef struct {
    struct component_t* content;
    int* scroll_offset;  // Pointer to external state
    int max_visible_height;
    bool show_indicators;  // Whether to show scroll indicators (▲/▼)
} scrollview_data_t;

/**
 * Create a new component of the given type
 * Returns NULL on allocation failure
 */
struct component_t* component_create(component_type_t type);

/**
 * Add a child component to a container
 * Returns true on success, false on failure
 */
bool component_add_child(struct component_t* parent, struct component_t* child);

/**
 * Recursively free a component and all its children
 */
void component_free(struct component_t* component);

/**
 * Set component data (takes ownership)
 */
void component_set_data(struct component_t* component, void* data);
