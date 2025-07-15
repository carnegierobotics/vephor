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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(TESTS
	gtest_union_find
	gtest_geometry
)

foreach (TEST ${TESTS})
	add_executable(${TEST} ${TEST}.cpp)
	include_directories(${TEST} ../../include)
	target_link_libraries(${TEST} ${GTEST_BOTH_LIBRARIES} vephor)
	add_test("${TEST}" ${TEST})
endforeach()

