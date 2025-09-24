cmake_minimum_required(VERSION 3.25)
project(intuitive_tui C)

set(CMAKE_C_STANDARD 23)

# Project-wide configurations
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# Include directories
include_directories(include)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(examples)
