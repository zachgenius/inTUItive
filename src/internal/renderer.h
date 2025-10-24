#pragma once

#include "component.h"
#include <stdbool.h>

/**
 * Renderer - walks component tree and draws to terminal
 */

/**
 * Render a component tree to the terminal
 * Recursively renders the component and all its children
 */
void render_component(struct component_t* component);

/**
 * Set clipping rectangle for rendering
 * Only content within this rectangle will be rendered
 */
void render_set_clip(int x, int y, int width, int height);

/**
 * Clear clipping rectangle
 */
void render_clear_clip(void);

/**
 * Check if a position is within the current clip rectangle
 */
bool render_is_clipped(int x, int y);
