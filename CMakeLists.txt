cmake_minimum_required(VERSION 3.31)
project(intuitive_tui C)

set(CMAKE_C_STANDARD 23)

add_library(intuitive_tui SHARED library.c)
