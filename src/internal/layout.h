#pragma once

#include "component.h"

/**
 * Layout system for calculating component sizes and positions
 */

/**
 * Measure phase: Calculate sizes for all components bottom-up
 * This computes the width and height of each component based on
 * its children and content.
 */
void layout_measure(struct component_t* component);

/**
 * Position phase: Assign positions to all components top-down
 * This sets the x,y coordinates for each component based on
 * its parent's position and layout rules.
 */
void layout_position(struct component_t* component, int x, int y);
