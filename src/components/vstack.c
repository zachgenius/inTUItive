#include "../include/intuitive.h"
#include "internal/component.h"
#include <stdarg.h>
#include <stdlib.h>

/**
 * Create a VStack (vertical stack) component
 * Uses variadic arguments terminated by NULL
 */
component_t* VStack(component_t* first, ...) {
    component_t* vstack = component_create(COMPONENT_VSTACK);
    if (!vstack) {
        return NULL;
    }

    // Add first child
    if (first) {
        component_add_child(vstack, first);

        // Add remaining children
        va_list args;
        va_start(args, first);

        component_t* child;
        while ((child = va_arg(args, component_t*)) != NULL) {
            component_add_child(vstack, child);
        }

        va_end(args);
    }

    return vstack;
}
