#pragma once

#include "component.h"

/**
 * Renderer - walks component tree and draws to terminal
 */

/**
 * Render a component tree to the terminal
 * Recursively renders the component and all its children
 */
void render_component(struct component_t* component);
