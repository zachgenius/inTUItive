#pragma once

#include "component.h"

/**
 * Build the focus list from a component tree
 * Must be called after building the tree, before rendering
 */
void focus_build_list(struct component_t* root);

/**
 * Move focus to the next focusable component
 * Returns true if focus changed
 */
bool focus_next(void);

/**
 * Move focus to the previous focusable component
 * Returns true if focus changed
 */
bool focus_prev(void);

/**
 * Get the currently focused component
 * Returns NULL if no component has focus
 */
struct component_t* focus_get_current(void);

/**
 * Clear all focus state
 */
void focus_clear(void);

/**
 * Set focus to a specific component
 * Returns true if focus was set successfully
 */
bool focus_set(struct component_t* component);
