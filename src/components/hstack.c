#include "../include/intuitive.h"
#include "internal/component.h"
#include <stdarg.h>

/**
 * Create an HStack (horizontal stack) component
 * Uses variadic arguments terminated by NULL
 */
component_t* HStack(component_t* first, ...) {
    component_t* hstack = component_create(COMPONENT_HSTACK);
    if (!hstack) {
        return NULL;
    }

    // Add first child
    if (first) {
        component_add_child(hstack, first);

        // Add remaining children
        va_list args;
        va_start(args, first);

        component_t* child;
        while ((child = va_arg(args, component_t*)) != NULL) {
            component_add_child(hstack, child);
        }

        va_end(args);
    }

    return hstack;
}
