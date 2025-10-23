#ifndef INTUITIVE_RENDERER_H
#define INTUITIVE_RENDERER_H

#include "component.h"

/**
 * Renderer - walks component tree and draws to terminal
 */

/**
 * Render a component tree to the terminal
 * Recursively renders the component and all its children
 */
void render_component(struct component_t* component);

#endif // INTUITIVE_RENDERER_H
