project(demo C)

# Option to choose between static and shared linking
option(USE_STATIC_LIB "Link with static library instead of shared" OFF)

# Hello World example (Phase 0 milestone)
add_executable(hello_world hello_world.c)

# Link with either static or shared library
if(USE_STATIC_LIB)
    target_link_libraries(hello_world intuitive_static)
else()
    target_link_libraries(hello_world intuitive)
endif()

# Counter example
add_executable(counter counter.c)

if(USE_STATIC_LIB)
    target_link_libraries(counter intuitive_static)
else()
    target_link_libraries(counter intuitive)
endif()

add_executable(todo_app todo_app.c)

if(USE_STATIC_LIB)
    target_link_libraries(todo_app intuitive_static)
else()
    target_link_libraries(todo_app intuitive)
endif()

# Styling demo example
add_executable(styling_demo styling_demo.c)

if(USE_STATIC_LIB)
    target_link_libraries(styling_demo intuitive_static)
else()
    target_link_libraries(styling_demo intuitive)
endif()

# Dashboard example (Phase 2 milestone)
add_executable(dashboard dashboard.c)

if(USE_STATIC_LIB)
    target_link_libraries(dashboard intuitive_static)
else()
    target_link_libraries(dashboard intuitive)
endif()

# ScrollView demo example
add_executable(scrollview_demo scrollview_demo.c)

if(USE_STATIC_LIB)
    target_link_libraries(scrollview_demo intuitive_static)
else()
    target_link_libraries(scrollview_demo intuitive)
endif()

# File Manager example (Phase 3 milestone)
add_executable(file_manager file_manager.c)

if(USE_STATIC_LIB)
    target_link_libraries(file_manager intuitive_static)
else()
    target_link_libraries(file_manager intuitive)
endif()

# Set output directory for examples
set_target_properties(hello_world counter todo_app styling_demo dashboard scrollview_demo file_manager PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)

# Installation rules for examples
install(TARGETS hello_world counter todo_app styling_demo dashboard scrollview_demo file_manager
    RUNTIME DESTINATION bin/examples
)