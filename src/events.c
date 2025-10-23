#include "internal/events.h"
#include <unistd.h>
#include <string.h>

bool event_poll(event_t* event) {
    char buf[8];

	// check the number of bytes read from the keyboard
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

    // Multi-byte sequences (arrow keys, etc.)
    if (buf[0] == '\033' && buf[1] == '[') {
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
