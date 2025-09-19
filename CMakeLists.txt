#
# Copyright 2025
# Carnegie Robotics, LLC
# 4501 Hatfield Street, Pittsburgh, PA 15201
# https://www.carnegierobotics.com
#
# This source code is licensed under the Apache License, Version 2.0
# found in the LICENSE file in the root directory of this source tree.
#

cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

project(vephor)

include(GNUInstallDirs)

option(VEPHOR_BUILD_PYTHON_BINDINGS "Build Python bindings with pybind11." OFF)
option(VEPHOR_BUILD_OPENGL "Build OpenGL portion of the project." ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

# Configure install paths based on whether this build is for Python bindings or not.
set(INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})
set(INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
set(INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})
set(INSTALL_DATADIR ${CMAKE_INSTALL_DATADIR})
if (VEPHOR_BUILD_PYTHON_BINDINGS)
    # This is a Python binding build, namespace the install paths so that everything isn't installed directly into
    # site-packages 
    string(PREPEND INSTALL_BINDIR "${PROJECT_NAME}/")
    string(PREPEND INSTALL_LIBDIR "${PROJECT_NAME}/")
    string(PREPEND INSTALL_INCLUDEDIR "${PROJECT_NAME}/")
    string(PREPEND INSTALL_DATADIR "${PROJECT_NAME}/")
endif ()

#
# Core library
#

add_subdirectory(core)

#
# OpenGL library
#

if (VEPHOR_BUILD_OPENGL)
    message("Building OpenGL.")
    add_subdirectory(opengl)
else()
    message("Not building OpenGL.")
endif()

#
# Python bindings
#

if (VEPHOR_BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
