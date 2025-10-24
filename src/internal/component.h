#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "../include/intuitive.h"
#include "animation.h"

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
    COMPONENT_TABLE,
    COMPONENT_PADDING,
    COMPONENT_SPACER,
    COMPONENT_SPINNER,
    COMPONENT_TOAST,
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

    // Diffing/reconciliation information
    bool dirty;  // Needs re-render
    unsigned int content_hash;  // Hash of component content for quick comparison
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
    int* scroll_offset;           // Pointer to external scroll state
    int max_visible_items;
    int* selected_index;         // Pointer to external selected index (optional)
    void (*on_select)(int index); // Callback when item selected (optional)

    // Smooth scrolling animation state
    float visual_scroll_offset;   // Current animated scroll position
    int target_scroll_offset;     // Target scroll position
    animation_t* scroll_animation; // Scroll animation (can be NULL)
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
    bool show_indicators;     // Whether to show scroll bar
    const char* thumb_focused;    // Character for thumb when focused
    const char* thumb_unfocused;  // Character for thumb when not focused
    const char* track_char;       // Character for track
    bool show_arrows;         // Whether to show ▲/▼ arrows

    // Smooth scrolling animation state
    float visual_scroll_offset;   // Current animated scroll position
    int target_scroll_offset;     // Target scroll position
    animation_t* scroll_animation; // Scroll animation (can be NULL)
} scrollview_data_t;

/**
 * Table component data
 */
typedef struct {
    char** headers;         // Array of header strings
    int header_count;       // Number of columns
    char*** rows;           // 2D array of cell strings [row][col]
    int row_count;          // Number of rows
    int* column_widths;     // Width of each column (auto-calculated)
    bool show_borders;      // Whether to show borders
} table_data_t;

/**
 * Stack component data (for aligned stacks)
 */
typedef struct {
    alignment_t alignment;
    int spacing;
} stack_data_t;

/**
 * Padding component data
 */
typedef struct {
    struct component_t* child;
    PaddingConfig padding;
} padding_data_t;

/**
 * Spinner component data
 */
typedef struct {
    spinner_style_t style;
    int frame_index;
    uint64_t last_update_time_us;
    int speed_ms;
    const char* text;
    float* progress;  // Optional: if set, shows progress percentage
} spinner_data_t;

/**
 * Toast component data
 */
typedef struct {
    char* message;
    bool* is_visible;
    toast_position_t position;
    void (*on_close)(void);
} toast_data_t;

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
