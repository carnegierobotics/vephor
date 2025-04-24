#
# Copyright 2023 - 2025
# Carnegie Robotics, LLC
# 4501 Hatfield Street, Pittsburgh, PA 15201
# https://www.carnegierobotics.com
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Carnegie Robotics, LLC nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL CARNEGIE ROBOTICS, LLC BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
    add_subdirectory(opengl)
endif()

#
# Python bindings
#

if (VEPHOR_BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()
