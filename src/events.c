#include "internal/events.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>

bool event_poll(event_t* event) {
    char buf[32];  // Increased for longer mouse sequences

    // Use select() with timeout to allow periodic wakeups
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;  // 100ms timeout

    int ready = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

    if (ready <= 0) {
        // Timeout or error - no event available
        event->type = EVENT_NONE;
        return false;
    }

    // Data available, read it
    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf) - 1);

    if (n <= 0) {
        event->type = EVENT_NONE;
        return false;
    }

    buf[n] = '\0';

    if (n == 1) {
        event->type = EVENT_KEY;
        event->data.key.code = (unsigned char)buf[0];
        return true;
    }

    // Multi-byte sequences (arrow keys, mouse, etc.)
    if (buf[0] == '\033' && buf[1] == '[') {
        // SGR Mouse event: \033[<button;x;y(M|m)
        if (buf[2] == '<') {
            int button, x, y;
            char action;

            // Parse: <button;x;yM or <button;x;ym
            if (sscanf(buf + 3, "%d;%d;%d%c", &button, &x, &y, &action) == 4) {
                event->type = EVENT_MOUSE;
                event->data.mouse.button = (mouse_button_t)button;
                event->data.mouse.x = x - 1;  // Convert to 0-based
                event->data.mouse.y = y - 1;  // Convert to 0-based

                // M = press, m = release
                if (action == 'M') {
                    event->data.mouse.action = MOUSE_PRESS;
                } else if (action == 'm') {
                    event->data.mouse.action = MOUSE_RELEASE;
                } else {
                    event->data.mouse.action = MOUSE_DRAG;
                }

                return true;
            }
        }

        // Regular escape sequences (arrow keys, etc.)
        switch (buf[2]) {
            case 'A':
                event->type = EVENT_KEY;
                event->data.key.code = KEY_UP;
                return true;
            case 'B':
                event->type = EVENT_KEY;
                event->data.key.code = KEY_DOWN;
                return true;
            case 'C':
                event->type = EVENT_KEY;
                event->data.key.code = KEY_RIGHT;
                return true;
            case 'D':
                event->type = EVENT_KEY;
                event->data.key.code = KEY_LEFT;
                return true;
            case '3':
                if (buf[3] == '~') {
                    event->type = EVENT_KEY;
                    event->data.key.code = KEY_DELETE;
                    return true;
                }
                break;
        	default:
        		break;
        }
    }

    event->type = EVENT_NONE;
    return false;
}
