#pragma once

#include <stdbool.h>

#define KEY_UP 256
#define KEY_DOWN 257
#define KEY_LEFT 258
#define KEY_RIGHT 259
#define KEY_ENTER 10
#define KEY_ESC 27
#define KEY_TAB 9
#define KEY_BACKSPACE 127
#define KEY_DELETE 330

typedef enum {
    EVENT_NONE,
    EVENT_KEY,
    EVENT_QUIT,
} event_type_t;

typedef struct {
    event_type_t type;
    union {
        struct {
            int code;
        } key;
    } data;
} event_t;

/**
 * Poll for an event (non-blocking)
 * Returns true if an event was received
 */
bool event_poll(event_t* event);
