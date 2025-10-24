#include "internal/animation.h"
#include <stdlib.h>
#include <math.h>

uint64_t anim_get_time_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec;
}

float anim_delta_time(uint64_t start_us, uint64_t end_us) {
    return (float)(end_us - start_us) / 1000000.0f;
}

bool anim_should_render_frame(uint64_t* last_frame_time_us, int target_fps) {
    if (!last_frame_time_us) {
        return false;
    }

    uint64_t now = anim_get_time_us();
    uint64_t frame_duration_us = 1000000 / target_fps;

    if (now - *last_frame_time_us >= frame_duration_us) {
        *last_frame_time_us = now;
        return true;
    }

    return false;
}

float ease_linear(float t) {
    return t;
}

float ease_in(float t) {
    return t * t;
}

float ease_out(float t) {
    return t * (2.0f - t);
}

float ease_in_out(float t) {
    if (t < 0.5f) {
        return 2.0f * t * t;
    } else {
        return -1.0f + (4.0f - 2.0f * t) * t;
    }
}

easing_fn_t get_easing_function(easing_type_t type) {
    switch (type) {
        case EASE_LINEAR:
            return ease_linear;
        case EASE_IN:
            return ease_in;
        case EASE_OUT:
            return ease_out;
        case EASE_IN_OUT:
            return ease_in_out;
        default:
            return ease_linear;
    }
}

animation_t* anim_create(float start, float end, uint64_t duration_ms, easing_type_t easing) {
    animation_t* anim = (animation_t*)calloc(1, sizeof(animation_t));
    if (!anim) {
        return NULL;
    }

    anim->start_value = start;
    anim->end_value = end;
    anim->current_value = start;
    anim->duration_us = duration_ms * 1000;
    anim->easing = get_easing_function(easing);
    anim->active = false;
    anim->completed = false;
    anim->next = NULL;

    return anim;
}

void anim_free(animation_t* anim) {
    if (anim) {
        free(anim);
    }
}

void anim_start(animation_t* anim) {
    if (!anim) {
        return;
    }

    anim->start_time_us = anim_get_time_us();
    anim->current_value = anim->start_value;
    anim->active = true;
    anim->completed = false;
}

bool anim_update(animation_t* anim) {
    if (!anim || !anim->active || anim->completed) {
        return false;
    }

    uint64_t now = anim_get_time_us();
    uint64_t elapsed_us = now - anim->start_time_us;

    if (elapsed_us >= anim->duration_us) {
        anim->current_value = anim->end_value;
        anim->completed = true;
        anim->active = false;
        return false;
    }

    float t = (float)elapsed_us / (float)anim->duration_us;
    float eased_t = anim->easing(t);

    anim->current_value = anim->start_value + (anim->end_value - anim->start_value) * eased_t;

    return true;
}

float anim_get_value(animation_t* anim) {
    if (!anim) {
        return 0.0f;
    }
    return anim->current_value;
}

bool anim_is_complete(animation_t* anim) {
    if (!anim) {
        return true;
    }
    return anim->completed;
}

void anim_manager_init(animation_manager_t* manager) {
    if (!manager) {
        return;
    }

    manager->head = NULL;
    manager->last_update_time_us = anim_get_time_us();
}

void anim_manager_add(animation_manager_t* manager, animation_t* anim) {
    if (!manager || !anim) {
        return;
    }

    anim->next = manager->head;
    manager->head = anim;
}

bool anim_manager_update(animation_manager_t* manager) {
    if (!manager) {
        return false;
    }

    manager->last_update_time_us = anim_get_time_us();

    bool has_active = false;
    animation_t* current = manager->head;

    while (current) {
        if (anim_update(current)) {
            has_active = true;
        }
        current = current->next;
    }

    return has_active;
}

void anim_manager_cleanup(animation_manager_t* manager) {
    if (!manager) {
        return;
    }

    animation_t* current = manager->head;
    animation_t* prev = NULL;

    while (current) {
        if (current->completed) {
            animation_t* to_remove = current;
            current = current->next;

            if (prev) {
                prev->next = current;
            } else {
                manager->head = current;
            }

            anim_free(to_remove);
        } else {
            prev = current;
            current = current->next;
        }
    }
}

void anim_manager_free_all(animation_manager_t* manager) {
    if (!manager) {
        return;
    }

    animation_t* current = manager->head;

    while (current) {
        animation_t* next = current->next;
        anim_free(current);
        current = next;
    }

    manager->head = NULL;
}
