cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

project(vephor)

option(BUILD_PYTHON_BINDINGS "Build Python bindings with pybind11." OFF)

add_subdirectory(core)
add_subdirectory(opengl)

if (BUILD_PYTHON_BINDINGS)
  add_subdirectory(python)
endif()