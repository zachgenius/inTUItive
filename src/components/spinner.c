#include "../internal/component.h"
#include "../internal/animation.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static const char* spinner_frames_braille[] = {
    "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"
};

static const char* spinner_frames_classic[] = {
    "|", "/", "-", "\\"
};

static const char* spinner_frames_dots[] = {
    "⣾", "⣽", "⣻", "⢿", "⡿", "⣟", "⣯", "⣷"
};

static const char* spinner_frames_box[] = {
    "◰", "◳", "◲", "◱"
};

static const char* spinner_frames_arrow[] = {
    "←", "↖", "↑", "↗", "→", "↘", "↓", "↙"
};

static const char** get_spinner_frames(spinner_style_t style, int* frame_count) {
    switch (style) {
        case SPINNER_BRAILLE:
            *frame_count = 10;
            return spinner_frames_braille;
        case SPINNER_CLASSIC:
            *frame_count = 4;
            return spinner_frames_classic;
        case SPINNER_DOTS:
            *frame_count = 8;
            return spinner_frames_dots;
        case SPINNER_BOX:
            *frame_count = 4;
            return spinner_frames_box;
        case SPINNER_ARROW:
            *frame_count = 8;
            return spinner_frames_arrow;
        default:
            *frame_count = 10;
            return spinner_frames_braille;
    }
}

component_t* Spinner(SpinnerConfig config) {
    component_t* comp = component_create(COMPONENT_SPINNER);
    if (!comp) {
        return NULL;
    }

    spinner_data_t* data = (spinner_data_t*)malloc(sizeof(spinner_data_t));
    if (!data) {
        component_free(comp);
        return NULL;
    }

    data->style = config.style;
    data->frame_index = 0;
    data->last_update_time_us = anim_get_time_us();
    data->speed_ms = config.speed > 0 ? config.speed : 100;
    data->text = config.text;
    data->progress = config.progress;

    component_set_data(comp, data);

    return comp;
}
