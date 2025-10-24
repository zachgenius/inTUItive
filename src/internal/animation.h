#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>

/**
 * Easing function types
 */
typedef enum {
    EASE_LINEAR,
    EASE_IN,
    EASE_OUT,
    EASE_IN_OUT
} easing_type_t;

/**
 * Easing function pointer type
 * Takes normalized time [0.0, 1.0] and returns eased value [0.0, 1.0]
 */
typedef float (*easing_fn_t)(float t);

/**
 * Animation state for a single property
 */
typedef struct animation_t {
    float start_value;
    float end_value;
    float current_value;

    uint64_t start_time_us;  // microseconds
    uint64_t duration_us;    // microseconds

    easing_fn_t easing;

    bool active;
    bool completed;

    struct animation_t* next;  // Linked list
} animation_t;

/**
 * Animation timing system
 */

/**
 * Get current time in microseconds
 */
uint64_t anim_get_time_us(void);

/**
 * Calculate delta time between two timestamps (in seconds)
 */
float anim_delta_time(uint64_t start_us, uint64_t end_us);

/**
 * Check if enough time has passed for next frame (frame rate limiting)
 * Returns true if a frame should be rendered
 */
bool anim_should_render_frame(uint64_t* last_frame_time_us, int target_fps);

/**
 * Easing functions
 */

float ease_linear(float t);
float ease_in(float t);
float ease_out(float t);
float ease_in_out(float t);

/**
 * Get easing function by type
 */
easing_fn_t get_easing_function(easing_type_t type);

/**
 * Animation state management
 */

/**
 * Create a new animation
 */
animation_t* anim_create(float start, float end, uint64_t duration_ms, easing_type_t easing);

/**
 * Free an animation
 */
void anim_free(animation_t* anim);

/**
 * Start an animation
 */
void anim_start(animation_t* anim);

/**
 * Update an animation (call every frame)
 * Returns true if animation is still active
 */
bool anim_update(animation_t* anim);

/**
 * Get current interpolated value
 */
float anim_get_value(animation_t* anim);

/**
 * Check if animation is complete
 */
bool anim_is_complete(animation_t* anim);

/**
 * Animation manager for multiple animations
 */

typedef struct {
    animation_t* head;  // Linked list of active animations
    uint64_t last_update_time_us;
} animation_manager_t;

/**
 * Initialize animation manager
 */
void anim_manager_init(animation_manager_t* manager);

/**
 * Add animation to manager
 */
void anim_manager_add(animation_manager_t* manager, animation_t* anim);

/**
 * Update all animations in manager
 * Returns true if any animations are still active
 */
bool anim_manager_update(animation_manager_t* manager);

/**
 * Remove completed animations from manager
 */
void anim_manager_cleanup(animation_manager_t* manager);

/**
 * Free all animations in manager
 */
void anim_manager_free_all(animation_manager_t* manager);
