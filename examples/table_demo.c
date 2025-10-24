#include "intuitive.h"

component_t* app(void) {
    // Table with borders
    const char* headers1[] = {"Name", "Age", "City", "Status"};
    const char* row1[] = {"Alice", "30", "New York", "Active"};
    const char* row2[] = {"Bob", "25", "Los Angeles", "Active"};
    const char* row3[] = {"Charlie", "35", "Chicago", "Inactive"};
    const char** rows1[] = {row1, row2, row3};

    component_t* table1 = Table((TableConfig){
        .headers = headers1,
        .rows = rows1,
        .column_count = 4,
        .row_count = 3,
        .show_borders = true
    });

    // Table without borders
    const char* headers2[] = {"Product", "Price", "Stock"};
    const char* row4[] = {"Laptop", "$999", "15"};
    const char* row5[] = {"Mouse", "$29", "50"};
    const char* row6[] = {"Keyboard", "$79", "30"};
    const char** rows2[] = {row4, row5, row6};

    component_t* table2 = Table((TableConfig){
        .headers = headers2,
        .rows = rows2,
        .column_count = 3,
        .row_count = 3,
        .show_borders = false
    });

    return VStack(
        Text("=== Table Component Demo ===", &(TextConfig){
            .fg_color = COLOR_BRIGHT_CYAN,
            .style = STYLE_BOLD
        }),
        Text("", NULL),
        Text("Table with borders:", NULL),
        table1,
        Text("", NULL),
        Text("Table without borders:", NULL),
        table2,
        Text("", NULL),
        Text("Press 'q' to quit", &(TextConfig){ .fg_color = COLOR_BRIGHT_GREEN }),
        NULL
    );
}

int main(void) {
    tui_init();
    tui_set_root(app);
    tui_run();
    return 0;
}
