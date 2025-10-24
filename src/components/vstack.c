#include "../include/intuitive.h"
#include "internal/component.h"
#include <stdarg.h>
#include <stdlib.h>

component_t* VStack(component_t* first, ...) {
    component_t* vstack = component_create(COMPONENT_VSTACK);
    if (!vstack) {
        return NULL;
    }

    if (first) {
        component_add_child(vstack, first);

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

component_t* VStackArray(component_t** children) {
    component_t* vstack = component_create(COMPONENT_VSTACK);
    if (!vstack) {
        return NULL;
    }

    if (children) {
        for (int i = 0; children[i] != NULL; i++) {
            component_add_child(vstack, children[i]);
        }
    }

    return vstack;
}
