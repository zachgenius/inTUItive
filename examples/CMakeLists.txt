project(demo C)

# Demo application
add_executable(demo demo.c)
target_link_libraries(demo intuitive)

# Set output directory for examples
set_target_properties(demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)

# Installation rules for examples
install(TARGETS demo
    RUNTIME DESTINATION bin/examples
)