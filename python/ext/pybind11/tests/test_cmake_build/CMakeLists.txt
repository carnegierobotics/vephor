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

# Built-in in CMake 3.5+
include(CMakeParseArguments)

add_custom_target(test_cmake_build)

function(pybind11_add_build_test name)
  cmake_parse_arguments(ARG "INSTALL" "" "" ${ARGN})

  set(build_options "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")

  if(PYBIND11_FINDPYTHON)
    list(APPEND build_options "-DPYBIND11_FINDPYTHON=${PYBIND11_FINDPYTHON}")

    if(DEFINED Python_ROOT_DIR)
      list(APPEND build_options "-DPython_ROOT_DIR=${Python_ROOT_DIR}")
    endif()

    list(APPEND build_options "-DPython_EXECUTABLE=${Python_EXECUTABLE}")
  else()
    list(APPEND build_options "-DPYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}")
  endif()

  if(DEFINED CMAKE_CXX_STANDARD)
    list(APPEND build_options "-DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}")
  endif()

  if(NOT ARG_INSTALL)
    list(APPEND build_options "-Dpybind11_SOURCE_DIR=${pybind11_SOURCE_DIR}")
  else()
    list(APPEND build_options "-DCMAKE_PREFIX_PATH=${pybind11_BINARY_DIR}/mock_install")
  endif()

  add_custom_target(
    test_build_${name}
    ${CMAKE_CTEST_COMMAND}
    --build-and-test
    "${CMAKE_CURRENT_SOURCE_DIR}/${name}"
    "${CMAKE_CURRENT_BINARY_DIR}/${name}"
    --build-config
    Release
    --build-noclean
    --build-generator
    ${CMAKE_GENERATOR}
    $<$<BOOL:${CMAKE_GENERATOR_PLATFORM}>:--build-generator-platform>
    ${CMAKE_GENERATOR_PLATFORM}
    --build-makeprogram
    ${CMAKE_MAKE_PROGRAM}
    --build-target
    check_${name}
    --build-options
    ${build_options})
  if(ARG_INSTALL)
    add_dependencies(test_build_${name} mock_install)
  endif()
  add_dependencies(test_cmake_build test_build_${name})
endfunction()

possibly_uninitialized(PYTHON_MODULE_EXTENSION Python_INTERPRETER_ID)

pybind11_add_build_test(subdirectory_function)
pybind11_add_build_test(subdirectory_target)
if("${PYTHON_MODULE_EXTENSION}" MATCHES "pypy" OR "${Python_INTERPRETER_ID}" STREQUAL "PyPy")
  message(STATUS "Skipping embed test on PyPy")
else()
  pybind11_add_build_test(subdirectory_embed)
endif()

if(PYBIND11_INSTALL)
  add_custom_target(
    mock_install ${CMAKE_COMMAND} "-DCMAKE_INSTALL_PREFIX=${pybind11_BINARY_DIR}/mock_install" -P
                 "${pybind11_BINARY_DIR}/cmake_install.cmake")

  pybind11_add_build_test(installed_function INSTALL)
  pybind11_add_build_test(installed_target INSTALL)
  if(NOT ("${PYTHON_MODULE_EXTENSION}" MATCHES "pypy" OR "${Python_INTERPRETER_ID}" STREQUAL "PyPy"
         ))
    pybind11_add_build_test(installed_embed INSTALL)
  endif()
endif()

add_dependencies(check test_cmake_build)

add_subdirectory(subdirectory_target EXCLUDE_FROM_ALL)
add_subdirectory(subdirectory_embed EXCLUDE_FROM_ALL)
