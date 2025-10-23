#ifndef INTUITIVE_COMPONENT_H
#define INTUITIVE_COMPONENT_H

#include <stddef.h>
#include <stdbool.h>

/**
 * Component types
 */
typedef enum {
    COMPONENT_TEXT,
    COMPONENT_VSTACK,
    COMPONENT_HSTACK,
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
    int x, y;           // Position
    int width, height;  // Size
};

/**
 * Text component data
 */
typedef struct {
    char* content;
} text_data_t;

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

#endif // INTUITIVE_COMPONENT_H
