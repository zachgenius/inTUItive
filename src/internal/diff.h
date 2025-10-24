#pragma once

#include "component.h"

/**
 * Compute content hash for a component
 * This hash represents the component's "identity" for diffing purposes
 */
unsigned int component_compute_hash(struct component_t* component);

/**
 * Compare and diff two component trees
 * Marks components as dirty if they have changed
 * Returns true if any changes were detected
 */
bool component_diff_trees(struct component_t* old_tree, struct component_t* new_tree);

/**
 * Mark a component and all its ancestors as dirty
 */
void component_mark_dirty(struct component_t* component);

/**
 * Mark all components in a tree as dirty
 */
void component_mark_all_dirty(struct component_t* component);
