project(intuitive C)
# Source files for the main library
set(INTUITIVE_TUI_SOURCES
    library.c
)

# Create the main library
add_library(intuitive SHARED ${INTUITIVE_TUI_SOURCES})

# Set library properties
set_target_properties(intuitive PROPERTIES
    VERSION 0.1.0
    SOVERSION 0
    PUBLIC_HEADER "../include/library.h"
)

# Link libraries if needed
# target_link_libraries(intuitive_tui ${REQUIRED_LIBS})

# Installation rules
install(TARGETS intuitive
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/intuitive
)