#include "../include/intuitive.h"
#include "internal/component.h"

component_t* Spacer(void) {
    component_t* spacer = component_create(COMPONENT_SPACER);
    if (!spacer) {
        return NULL;
    }

    // Spacer has no data - its behavior is handled in layout
    return spacer;
}
