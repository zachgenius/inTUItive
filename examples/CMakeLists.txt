project(demo C)

# Option to choose between static and shared linking
option(USE_STATIC_LIB "Link with static library instead of shared" OFF)

# Demo application
add_executable(demo demo.c)

# Link with either static or shared library
if(USE_STATIC_LIB)
    target_link_libraries(demo intuitive_static)
else()
    target_link_libraries(demo intuitive)
endif()

# Set output directory for examples
set_target_properties(demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)

# Installation rules for examples
install(TARGETS demo
    RUNTIME DESTINATION bin/examples
)