cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

project(vephor)

option(BUILD_PYTHON_BINDINGS "Build Python bindings with pybind11." OFF)
option(BUILD_OPENGL "Build OpenGL portion of the project." ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(core)

if (BUILD_OPENGL)
  add_subdirectory(opengl)
endif()

if (BUILD_PYTHON_BINDINGS)
  add_subdirectory(python)
endif()