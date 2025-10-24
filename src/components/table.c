#include "intuitive.h"
#include "internal/component.h"
#include <stdlib.h>
#include <string.h>

component_t* Table(TableConfig config) {
    if (!config.headers || !config.rows || config.column_count <= 0 || config.row_count <= 0) {
        return NULL;
    }

    component_t* table = component_create(COMPONENT_TABLE);
    if (!table) {
        return NULL;
    }

    table_data_t* data = calloc(1, sizeof(table_data_t));
    if (!data) {
        component_free(table);
        return NULL;
    }

    // Copy headers
    data->headers = calloc(config.column_count, sizeof(char*));
    if (!data->headers) {
        free(data);
        component_free(table);
        return NULL;
    }

    for (int i = 0; i < config.column_count; i++) {
        data->headers[i] = strdup(config.headers[i]);
        if (!data->headers[i]) {
            for (int j = 0; j < i; j++) {
                free(data->headers[j]);
            }
            free(data->headers);
            free(data);
            component_free(table);
            return NULL;
        }
    }

    // Copy rows
    data->rows = calloc(config.row_count, sizeof(char**));
    if (!data->rows) {
        for (int i = 0; i < config.column_count; i++) {
            free(data->headers[i]);
        }
        free(data->headers);
        free(data);
        component_free(table);
        return NULL;
    }

    for (int row = 0; row < config.row_count; row++) {
        data->rows[row] = calloc(config.column_count, sizeof(char*));
        if (!data->rows[row]) {
            // Cleanup previously allocated rows
            for (int r = 0; r < row; r++) {
                for (int c = 0; c < config.column_count; c++) {
                    free(data->rows[r][c]);
                }
                free(data->rows[r]);
            }
            free(data->rows);
            for (int i = 0; i < config.column_count; i++) {
                free(data->headers[i]);
            }
            free(data->headers);
            free(data);
            component_free(table);
            return NULL;
        }

        for (int col = 0; col < config.column_count; col++) {
            data->rows[row][col] = strdup(config.rows[row][col]);
            if (!data->rows[row][col]) {
                // Cleanup
                for (int c = 0; c < col; c++) {
                    free(data->rows[row][c]);
                }
                for (int r = 0; r < row; r++) {
                    for (int c = 0; c < config.column_count; c++) {
                        free(data->rows[r][c]);
                    }
                    free(data->rows[r]);
                }
                free(data->rows[row]);
                free(data->rows);
                for (int i = 0; i < config.column_count; i++) {
                    free(data->headers[i]);
                }
                free(data->headers);
                free(data);
                component_free(table);
                return NULL;
            }
        }
    }

    data->header_count = config.column_count;
    data->row_count = config.row_count;
    data->show_borders = config.show_borders;

    // Calculate column widths (max of header and all rows)
    data->column_widths = calloc(config.column_count, sizeof(int));
    if (!data->column_widths) {
        // Cleanup all allocated memory
        for (int r = 0; r < config.row_count; r++) {
            for (int c = 0; c < config.column_count; c++) {
                free(data->rows[r][c]);
            }
            free(data->rows[r]);
        }
        free(data->rows);
        for (int i = 0; i < config.column_count; i++) {
            free(data->headers[i]);
        }
        free(data->headers);
        free(data);
        component_free(table);
        return NULL;
    }

    for (int col = 0; col < config.column_count; col++) {
        int max_width = strlen(config.headers[col]);
        for (int row = 0; row < config.row_count; row++) {
            int cell_width = strlen(config.rows[row][col]);
            if (cell_width > max_width) {
                max_width = cell_width;
            }
        }
        data->column_widths[col] = max_width;
    }

    component_set_data(table, data);
    return table;
}
