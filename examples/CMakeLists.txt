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

# Set output directory for examples
set_target_properties(hello_world PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)

# Installation rules for examples
install(TARGETS hello_world
    RUNTIME DESTINATION bin/examples
)