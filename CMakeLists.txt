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

# Platform detection and configuration
if(WIN32)
    # Windows
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(UNIX AND NOT APPLE)
    # Linux
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
elseif(APPLE)
    # macOS
    set(CMAKE_MACOSX_RPATH ON)
endif()

# Compiler flags
if(MSVC)
    # Microsoft Visual C++
    set(CMAKE_C_FLAGS_DEBUG "/Zi /Od /Wall")
    set(CMAKE_C_FLAGS_RELEASE "/O2 /DNDEBUG")
else()
    # GCC/Clang
    set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
    set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

    # Additional warnings for better code quality
    add_compile_options(-Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition)
endif()

# Include directories
include_directories(include)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(examples)
