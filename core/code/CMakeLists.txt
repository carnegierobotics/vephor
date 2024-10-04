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

cmake_minimum_required(VERSION 3.16)

project(vephor VERSION 0.1)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# Some cmake sections adapted from:
#	https://stackoverflow.com/questions/47718485/install-and-export-interface-only-library-cmake
#	https://dominikberner.ch/cmake-interface-lib/

find_package(Eigen3 3.3 REQUIRED)
find_package(manif REQUIRED)

set(VEPHOR_USE_EXTERNAL_JSON OFF CACHE BOOL "Use an external JSON package")

add_library(vephor INTERFACE)
target_include_directories(vephor INTERFACE 
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(vephor INTERFACE Eigen3::Eigen MANIF::manif)

if(VEPHOR_USE_EXTERNAL_JSON)
	message("Vephor using external JSON.")
    find_package(nlohmann_json REQUIRED)
	target_link_libraries(vephor INTERFACE nlohmann_json::nlohmann_json)
	target_compile_definitions(vephor INTERFACE USE_EXTERNAL_JSON)
endif()

target_compile_options(
	vephor
	INTERFACE "-Wno-sign-compare"
			"-Wno-missing-field-initializers")
	
include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
								 VERSION ${PROJECT_VERSION}
								 COMPATIBILITY AnyNewerVersion)

if (!VEPHOR_BUILD_PYTHON_BINDINGS)

set(EXAMPLE_APPS
	test_show
	mini_test_show
	mini_test_show_transparent
	mini_test_server
	test_plot
	test_imshow
	test_plot_render
	test_show_two_windows
	produce_sprites_with_normals
)

foreach (APP ${EXAMPLE_APPS})
	add_executable (${APP} examples/${APP}.cpp)
	target_link_libraries(${APP} vephor)
	#include_directories(${APP} include)
	
	if (WIN32)
		target_link_libraries(${APP} Ws2_32)
	else()
		target_link_libraries(${APP} pthread)
	endif()
endforeach()

set(EXAMPLE_NETWORKING_APPS
	test_client
	test_client_jsonb
	test_server
	test_server_jsonb
	test_server_daemon_jsonb
)

foreach (APP ${EXAMPLE_NETWORKING_APPS})
	add_executable (networking_${APP} examples/networking/${APP}.cpp)
	target_link_libraries(networking_${APP} vephor)
	
	if (WIN32)
		target_link_libraries(networking_${APP} Ws2_32)
	else()
		target_link_libraries(networking_${APP} pthread)
	endif()
endforeach()

endif()


install(TARGETS vephor
    EXPORT vephor_targets
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    RUNTIME DESTINATION bin COMPONENT Runtime
    PUBLIC_HEADER DESTINATION include COMPONENT Development
    BUNDLE DESTINATION bin COMPONENT Runtime)

install(EXPORT vephor_targets DESTINATION lib/cmake/vephor)
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        DESTINATION lib/cmake/vephor)
install(DIRECTORY include DESTINATION .)

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION
  ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)


set(BUILD_UNIT_TESTS OFF CACHE BOOL "Build unit tests")

if (BUILD_UNIT_TESTS)
    find_package(GTest)
    if(GTEST_FOUND)
        add_subdirectory(test/unit)
    else(GTEST_FOUND)
        message(WARNING "GTest not found, tests will not be built.")
    endif()
endif()