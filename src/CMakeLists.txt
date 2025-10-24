project(intuitive C)
# Source files for the main library
set(INTUITIVE_TUI_SOURCES
    terminal.c
    component.c
    layout.c
    renderer.c
    tui.c
    events.c
    focus.c
    components/text.c
    components/vstack.c
    components/hstack.c
    components/button.c
    components/input.c
    components/style.c
    components/list.c
    components/modal.c
    components/scrollview.c
    components/table.c
)

# Create both shared and static libraries
add_library(intuitive SHARED ${INTUITIVE_TUI_SOURCES})
add_library(intuitive_static STATIC ${INTUITIVE_TUI_SOURCES})

# Add include directories
target_include_directories(intuitive PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(intuitive_static PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Set the same output name for both libraries
set_target_properties(intuitive_static PROPERTIES OUTPUT_NAME intuitive)

# Set library properties for shared library
set_target_properties(intuitive PROPERTIES
    VERSION 0.1.0
    SOVERSION 0
    PUBLIC_HEADER "../include/intuitive.h"
)

# Set library properties for static library
set_target_properties(intuitive_static PROPERTIES
    VERSION 0.1.0
    PUBLIC_HEADER "../include/intuitive.h"
)

# Platform-specific configurations
if(WIN32)
    # Windows-specific settings
    set_target_properties(intuitive PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
    target_compile_definitions(intuitive PRIVATE INTUITIVE_BUILDING_DLL)
    target_compile_definitions(intuitive_static PRIVATE INTUITIVE_STATIC)
elseif(UNIX AND NOT APPLE)
    # Linux-specific settings
    set_target_properties(intuitive PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
elseif(APPLE)
    # macOS-specific settings
    set_target_properties(intuitive PROPERTIES
        MACOSX_RPATH ON
        INSTALL_RPATH "@rpath"
    )
endif()

# Link libraries if needed
# target_link_libraries(intuitive ${REQUIRED_LIBS})
# target_link_libraries(intuitive_static ${REQUIRED_LIBS})

# Installation rules
install(TARGETS intuitive intuitive_static
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include/intuitive
)