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
    EVENT_MOUSE,
    EVENT_QUIT,
} event_type_t;

typedef enum {
    MOUSE_LEFT = 0,
    MOUSE_MIDDLE = 1,
    MOUSE_RIGHT = 2,
    MOUSE_SCROLL_UP = 64,
    MOUSE_SCROLL_DOWN = 65,
} mouse_button_t;

typedef enum {
    MOUSE_PRESS,
    MOUSE_RELEASE,
    MOUSE_DRAG,
} mouse_action_t;

typedef struct {
    event_type_t type;
    union {
        struct {
            int code;
        } key;
        struct {
            mouse_button_t button;
            mouse_action_t action;
            int x;  // Column (0-based)
            int y;  // Row (0-based)
        } mouse;
    } data;
} event_t;

/**
 * Poll for an event (non-blocking)
 * Returns true if an event was received
 */
bool event_poll(event_t* event);
