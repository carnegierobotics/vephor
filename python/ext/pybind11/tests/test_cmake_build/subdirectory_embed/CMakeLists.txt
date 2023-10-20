#
# Copyright 2023
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

cmake_minimum_required(VERSION 3.4)

# The `cmake_minimum_required(VERSION 3.4...3.18)` syntax does not work with
# some versions of VS that have a patched CMake 3.11. This forces us to emulate
# the behavior using the following workaround:
if(${CMAKE_VERSION} VERSION_LESS 3.18)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
  cmake_policy(VERSION 3.18)
endif()

project(test_subdirectory_embed CXX)

set(PYBIND11_INSTALL
    ON
    CACHE BOOL "")
set(PYBIND11_EXPORT_NAME test_export)

add_subdirectory("${pybind11_SOURCE_DIR}" pybind11)

# Test basic target functionality
add_executable(test_subdirectory_embed ../embed.cpp)
target_link_libraries(test_subdirectory_embed PRIVATE pybind11::embed)
set_target_properties(test_subdirectory_embed PROPERTIES OUTPUT_NAME test_cmake_build)

add_custom_target(
  check_subdirectory_embed
  $<TARGET_FILE:test_subdirectory_embed> "${PROJECT_SOURCE_DIR}/../test.py"
  DEPENDS test_subdirectory_embed)

# Test custom export group -- PYBIND11_EXPORT_NAME
add_library(test_embed_lib ../embed.cpp)
target_link_libraries(test_embed_lib PRIVATE pybind11::embed)

install(
  TARGETS test_embed_lib
  EXPORT test_export
  ARCHIVE DESTINATION bin
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib)
install(EXPORT test_export DESTINATION lib/cmake/test_export/test_export-Targets.cmake)
