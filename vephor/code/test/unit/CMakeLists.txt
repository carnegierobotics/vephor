cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(TESTS
	gtest_union_find
	gtest_geometry
)

foreach (TEST ${TESTS})
	add_executable(${TEST} ${TEST}.cpp)
	include_directories(${TEST} ../../include)
	target_link_libraries(${TEST} ${GTEST_BOTH_LIBRARIES})
	add_test("${TEST}" ${TEST})
endforeach()

