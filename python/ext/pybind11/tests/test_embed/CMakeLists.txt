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

possibly_uninitialized(PYTHON_MODULE_EXTENSION Python_INTERPRETER_ID)

if("${PYTHON_MODULE_EXTENSION}" MATCHES "pypy" OR "${Python_INTERPRETER_ID}" STREQUAL "PyPy")
  message(STATUS "Skipping embed test on PyPy")
  add_custom_target(cpptest) # Dummy target on PyPy. Embedding is not supported.
  set(_suppress_unused_variable_warning "${DOWNLOAD_CATCH}")
  return()
endif()

find_package(Catch 2.13.9)

if(CATCH_FOUND)
  message(STATUS "Building interpreter tests using Catch v${CATCH_VERSION}")
else()
  message(STATUS "Catch not detected. Interpreter tests will be skipped. Install Catch headers"
                 " manually or use `cmake -DDOWNLOAD_CATCH=ON` to fetch them automatically.")
  return()
endif()

find_package(Threads REQUIRED)

add_executable(test_embed catch.cpp test_interpreter.cpp)
pybind11_enable_warnings(test_embed)

target_link_libraries(test_embed PRIVATE pybind11::embed Catch2::Catch2 Threads::Threads)

if(NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
  file(COPY test_interpreter.py test_trampoline.py DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()

add_custom_target(
  cpptest
  COMMAND "$<TARGET_FILE:test_embed>"
  DEPENDS test_embed
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

pybind11_add_module(external_module THIN_LTO external_module.cpp)
set_target_properties(external_module PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                                 "${CMAKE_CURRENT_BINARY_DIR}")
foreach(config ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${config} config)
  set_target_properties(external_module PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${config}
                                                   "${CMAKE_CURRENT_BINARY_DIR}")
endforeach()
add_dependencies(cpptest external_module)

add_dependencies(check cpptest)
