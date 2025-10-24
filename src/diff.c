#include "internal/diff.h"
#include "internal/component.h"
#include <string.h>
#include <stdbool.h>

/**
 * Simple hash function (DJB2)
 */
static unsigned int hash_string(const char* str) {
    unsigned int hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

/**
 * Hash an integer value
 */
static unsigned int hash_int(int value) {
    unsigned int hash = (unsigned int)value;
    hash = ((hash >> 16) ^ hash) * 0x45d9f3b;
    hash = ((hash >> 16) ^ hash) * 0x45d9f3b;
    hash = (hash >> 16) ^ hash;
    return hash;
}

/**
 * Combine two hashes
 */
static unsigned int hash_combine(unsigned int h1, unsigned int h2) {
    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
}

/**
 * Compute hash for component data based on type
 */
static unsigned int hash_component_data(struct component_t* component) {
    if (!component || !component->data) {
        return 0;
    }

    unsigned int hash = 0;

    switch (component->type) {
        case COMPONENT_TEXT: {
            text_data_t* data = (text_data_t*)component->data;
            if (data->content) {
                hash = hash_string(data->content);
            }
            break;
        }

        case COMPONENT_BUTTON: {
            button_data_t* data = (button_data_t*)component->data;
            if (data->label) {
                hash = hash_string(data->label);
            }
            // Note: We don't hash the callback pointer since it doesn't affect rendering
            break;
        }

        case COMPONENT_INPUT: {
            input_data_t* data = (input_data_t*)component->data;
            if (data->buffer) {
                hash = hash_string(data->buffer);
            }
            hash = hash_combine(hash, hash_int(data->cursor_pos));
            break;
        }

        case COMPONENT_LIST: {
            list_data_t* data = (list_data_t*)component->data;
            // Hash item count and selected index
            hash = hash_int(data->item_count);
            if (data->selected_index) {
                hash = hash_combine(hash, hash_int(*data->selected_index));
            }
            if (data->scroll_offset) {
                hash = hash_combine(hash, hash_int(*data->scroll_offset));
            }
            // Hash each item string
            for (int i = 0; i < data->item_count; i++) {
                if (data->items[i]) {
                    hash = hash_combine(hash, hash_string(data->items[i]));
                }
            }
            break;
        }

        case COMPONENT_MODAL: {
            modal_data_t* data = (modal_data_t*)component->data;
            if (data->is_open) {
                hash = hash_int(*data->is_open ? 1 : 0);
            }
            if (data->title) {
                hash = hash_combine(hash, hash_string(data->title));
            }
            break;
        }

        case COMPONENT_SCROLLVIEW: {
            scrollview_data_t* data = (scrollview_data_t*)component->data;
            if (data->scroll_offset) {
                hash = hash_int(*data->scroll_offset);
            }
            hash = hash_combine(hash, hash_int(data->max_visible_height));
            break;
        }

        case COMPONENT_TABLE: {
            table_data_t* data = (table_data_t*)component->data;
            hash = hash_int(data->row_count);
            hash = hash_combine(hash, hash_int(data->header_count));
            hash = hash_combine(hash, hash_int(data->show_borders ? 1 : 0));
            // Hash headers
            for (int i = 0; i < data->header_count; i++) {
                if (data->headers[i]) {
                    hash = hash_combine(hash, hash_string(data->headers[i]));
                }
            }
            // Hash cells (sample first few rows to avoid being too slow)
            int rows_to_hash = data->row_count < 10 ? data->row_count : 10;
            for (int r = 0; r < rows_to_hash; r++) {
                for (int c = 0; c < data->header_count; c++) {
                    if (data->rows[r] && data->rows[r][c]) {
                        hash = hash_combine(hash, hash_string(data->rows[r][c]));
                    }
                }
            }
            break;
        }

        case COMPONENT_PADDING: {
            padding_data_t* data = (padding_data_t*)component->data;
            hash = hash_int(data->padding.top);
            hash = hash_combine(hash, hash_int(data->padding.bottom));
            hash = hash_combine(hash, hash_int(data->padding.left));
            hash = hash_combine(hash, hash_int(data->padding.right));
            break;
        }

        case COMPONENT_SPINNER: {
            spinner_data_t* data = (spinner_data_t*)component->data;
            hash = hash_int(data->style);
            if (data->text) {
                hash = hash_combine(hash, hash_string(data->text));
            }
            if (data->progress) {
                hash = hash_combine(hash, hash_int((int)(*data->progress * 10)));
            }
            // Note: frame_index and timing info are animation state, not content
            break;
        }

        case COMPONENT_TOAST: {
            toast_data_t* data = (toast_data_t*)component->data;
            if (data->message) {
                hash = hash_string(data->message);
            }
            hash = hash_combine(hash, hash_int(data->position));
            if (data->is_visible) {
                hash = hash_combine(hash, hash_int(*data->is_visible ? 1 : 0));
            }
            break;
        }

        case COMPONENT_VSTACK:
        case COMPONENT_HSTACK:
            // For stacks, hash alignment and spacing if they have data
            if (component->data) {
                stack_data_t* data = (stack_data_t*)component->data;
                hash = hash_int(data->alignment);
                hash = hash_combine(hash, hash_int(data->spacing));
            }
            break;

        case COMPONENT_SPACER:
            // Spacer has no data
            break;
    }

    return hash;
}

unsigned int component_compute_hash(struct component_t* component) {
    if (!component) {
        return 0;
    }

    unsigned int hash = hash_int(component->type);

    // Hash style information
    hash = hash_combine(hash, hash_int(component->fg_color));
    hash = hash_combine(hash, hash_int(component->bg_color));
    hash = hash_combine(hash, hash_int(component->style));

    // Hash focus state
    hash = hash_combine(hash, hash_int(component->focusable ? 1 : 0));
    hash = hash_combine(hash, hash_int(component->focused ? 1 : 0));

    // Hash component data
    hash = hash_combine(hash, hash_component_data(component));

    // Hash child count (child content is hashed separately)
    hash = hash_combine(hash, hash_int(component->child_count));

    component->content_hash = hash;
    return hash;
}

bool component_diff_trees(struct component_t* old_tree, struct component_t* new_tree) {
    if (!new_tree) {
        return false;
    }

    // If no old tree, mark everything as dirty
    if (!old_tree) {
        component_mark_all_dirty(new_tree);
        return true;
    }

    bool has_changes = false;

    // Check if types differ
    if (old_tree->type != new_tree->type) {
        new_tree->dirty = true;
        return true;
    }

    // Compute hashes
    component_compute_hash(old_tree);
    component_compute_hash(new_tree);

    // Compare hashes
    if (old_tree->content_hash != new_tree->content_hash) {
        new_tree->dirty = true;
        has_changes = true;
    }

    // Check layout changes (position/size changes need re-render)
    if (old_tree->x != new_tree->x || old_tree->y != new_tree->y ||
        old_tree->width != new_tree->width || old_tree->height != new_tree->height) {
        new_tree->dirty = true;
        has_changes = true;
    }

    // Preserve animation state for components that need it
    if (old_tree->type == COMPONENT_SPINNER && new_tree->type == COMPONENT_SPINNER) {
        spinner_data_t* old_data = (spinner_data_t*)old_tree->data;
        spinner_data_t* new_data = (spinner_data_t*)new_tree->data;
        if (old_data && new_data) {
            // Preserve animation frame and timing state
            new_data->frame_index = old_data->frame_index;
            new_data->last_update_time_us = old_data->last_update_time_us;
        }
    } else if (old_tree->type == COMPONENT_LIST && new_tree->type == COMPONENT_LIST) {
        list_data_t* old_data = (list_data_t*)old_tree->data;
        list_data_t* new_data = (list_data_t*)new_tree->data;
        if (old_data && new_data) {
            // Preserve scroll animation state
            new_data->visual_scroll_offset = old_data->visual_scroll_offset;
            new_data->target_scroll_offset = old_data->target_scroll_offset;
            new_data->scroll_animation = old_data->scroll_animation;
            // Prevent double-free by nulling out old animation
            old_data->scroll_animation = NULL;
        }
    } else if (old_tree->type == COMPONENT_SCROLLVIEW && new_tree->type == COMPONENT_SCROLLVIEW) {
        scrollview_data_t* old_data = (scrollview_data_t*)old_tree->data;
        scrollview_data_t* new_data = (scrollview_data_t*)new_tree->data;
        if (old_data && new_data) {
            // Preserve scroll animation state
            new_data->visual_scroll_offset = old_data->visual_scroll_offset;
            new_data->target_scroll_offset = old_data->target_scroll_offset;
            new_data->scroll_animation = old_data->scroll_animation;
            // Prevent double-free by nulling out old animation
            old_data->scroll_animation = NULL;
        }
    }

    // Recursively diff children
    // For simplicity, if child count changed, mark as dirty
    if (old_tree->child_count != new_tree->child_count) {
        new_tree->dirty = true;
        has_changes = true;
        // Still diff matching children
        int min_children = old_tree->child_count < new_tree->child_count ?
                          old_tree->child_count : new_tree->child_count;
        for (int i = 0; i < min_children; i++) {
            component_diff_trees(old_tree->children[i], new_tree->children[i]);
        }
        // Mark new children as dirty
        for (int i = min_children; i < new_tree->child_count; i++) {
            component_mark_all_dirty(new_tree->children[i]);
        }
    } else {
        // Same child count, diff each child
        for (int i = 0; i < new_tree->child_count; i++) {
            if (component_diff_trees(old_tree->children[i], new_tree->children[i])) {
                has_changes = true;
            }
        }
    }

    // Handle special container components with non-child content
    if (new_tree->type == COMPONENT_MODAL) {
        modal_data_t* old_data = (modal_data_t*)old_tree->data;
        modal_data_t* new_data = (modal_data_t*)new_tree->data;
        if (old_data && new_data && old_data->content && new_data->content) {
            if (component_diff_trees(old_data->content, new_data->content)) {
                new_tree->dirty = true;
                has_changes = true;
            }
        }
    } else if (new_tree->type == COMPONENT_SCROLLVIEW) {
        scrollview_data_t* old_data = (scrollview_data_t*)old_tree->data;
        scrollview_data_t* new_data = (scrollview_data_t*)new_tree->data;
        if (old_data && new_data && old_data->content && new_data->content) {
            if (component_diff_trees(old_data->content, new_data->content)) {
                new_tree->dirty = true;
                has_changes = true;
            }
        }
    } else if (new_tree->type == COMPONENT_PADDING) {
        padding_data_t* old_data = (padding_data_t*)old_tree->data;
        padding_data_t* new_data = (padding_data_t*)new_tree->data;
        if (old_data && new_data && old_data->child && new_data->child) {
            if (component_diff_trees(old_data->child, new_data->child)) {
                new_tree->dirty = true;
                has_changes = true;
            }
        }
    }

    // If any children changed, parent is dirty (needs re-render for proper layout)
    if (has_changes && !new_tree->dirty) {
        new_tree->dirty = true;
    }

    return has_changes || new_tree->dirty;
}

void component_mark_dirty(struct component_t* component) {
    if (!component) {
        return;
    }
    component->dirty = true;
}

void component_mark_all_dirty(struct component_t* component) {
    if (!component) {
        return;
    }

    component->dirty = true;

    // Recursively mark children
    for (int i = 0; i < component->child_count; i++) {
        component_mark_all_dirty(component->children[i]);
    }

    // Mark special container content
    if (component->type == COMPONENT_MODAL) {
        modal_data_t* data = (modal_data_t*)component->data;
        if (data && data->content) {
            component_mark_all_dirty(data->content);
        }
    } else if (component->type == COMPONENT_SCROLLVIEW) {
        scrollview_data_t* data = (scrollview_data_t*)component->data;
        if (data && data->content) {
            component_mark_all_dirty(data->content);
        }
    } else if (component->type == COMPONENT_PADDING) {
        padding_data_t* data = (padding_data_t*)component->data;
        if (data && data->child) {
            component_mark_all_dirty(data->child);
        }
    }
}
