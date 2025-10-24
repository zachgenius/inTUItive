#include "intuitive.h"
#include "internal/component.h"
#include <stdlib.h>

component_t* Color(component_t* comp, color_t fg, color_t bg) {
    if (!comp) {
        return NULL;
    }

    comp->fg_color = fg;
    comp->bg_color = bg;
    return comp;
}

component_t* FgColor(component_t* comp, color_t fg) {
    if (!comp) {
        return NULL;
    }

    comp->fg_color = fg;
    return comp;
}

component_t* BgColor(component_t* comp, color_t bg) {
    if (!comp) {
        return NULL;
    }

    comp->bg_color = bg;
    return comp;
}

component_t* Bold(component_t* comp) {
    if (!comp) {
        return NULL;
    }

    comp->style |= STYLE_BOLD;
    return comp;
}

component_t* Underline(component_t* comp) {
    if (!comp) {
        return NULL;
    }

    comp->style |= STYLE_UNDERLINE;
    return comp;
}
