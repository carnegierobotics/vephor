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

#
# Some cmake sections adapted from:
#	https://stackoverflow.com/questions/47718485/install-and-export-interface-only-library-cmake
#	https://dominikberner.ch/cmake-interface-lib/
#	https://discourse.cmake.org/t/understanding-the-cmake-component-keyword-in-the-install-command/971
#

# Make CMake scripts available
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(CMakePackageConfigHelpers)

option(VEPHOR_BUILD_TESTING "Enable testing" OFF)

#
# vephor
#

include(${PROJECT_NAME}-dependencies)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME}
	INTERFACE 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:${INSTALL_INCLUDEDIR}>
)
set_property(TARGET ${PROJECT_NAME}
    APPEND PROPERTY PUBLIC_HEADER
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
)  # TODO: Should public headers be specified granularly?
target_link_libraries(${PROJECT_NAME}
	INTERFACE
		Eigen3::Eigen
		MANIF::manif
)

# Delegate to either the internal or an external Nlohmann JSON library
set(VEPHOR_USE_EXTERNAL_JSON OFF CACHE BOOL "Use an external JSON package")
if (VEPHOR_USE_EXTERNAL_JSON)
	if (nlohmann_json_FOUND)
		message(STATUS "Vephor using external JSON.")
		target_link_libraries(${PROJECT_NAME}
			INTERFACE
				nlohmann_json::nlohmann_json
		)
		target_compile_definitions(${PROJECT_NAME}
			INTERFACE
				USE_EXTERNAL_JSON
		)
	else ()
		message(WARNING "External Nlohmann JSON not found.  Using internal library.")
	endif ()
endif ()

target_compile_options(${PROJECT_NAME}
	INTERFACE
		"-Wno-sign-compare"
		"-Wno-missing-field-initializers"
)

install(
	TARGETS ${PROJECT_NAME}
	EXPORT ${PROJECT_NAME}-targets
	LIBRARY
		DESTINATION ${INSTALL_LIBDIR}
		COMPONENT Runtime
	ARCHIVE
		DESTINATION ${INSTALL_LIBDIR}
		COMPONENT Development
	RUNTIME
		DESTINATION ${INSTALL_BINDIR}
		COMPONENT Runtime
	PUBLIC_HEADER
		DESTINATION ${INSTALL_INCLUDEDIR}
		COMPONENT Development
	BUNDLE
		DESTINATION ${INSTALL_BINDIR}
		COMPONENT Runtime
)
install(
	EXPORT ${PROJECT_NAME}-targets
	DESTINATION ${INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# Package configuration and version files
configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
)
write_basic_package_version_file(
	${PROJECT_NAME}-config-version.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
)
install(
	FILES
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
		${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}-dependencies.cmake
	DESTINATION ${INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

#
# Example applications
#

if (VEPHOR_BUILD_PYTHON_BINDINGS)
	message(STATUS "Not building examples due to python bindings.")
else ()
	if (WIN32)
		set(OS_LIBS Ws2_32)
	else()
		set(OS_LIBS pthread)
	endif()

	# Applications
	set(EXAMPLE_APPS
		test_show
		mini_test_show
		mini_test_show_transparent
		mini_test_show_trajectory_camera
		mini_test_server
		test_plot
		test_scatter
		test_scatter_set
		test_imshow
		test_plot_render
		test_show_two_windows
		produce_sprites_with_normals
		grid_counter
	)
	foreach (APP ${EXAMPLE_APPS})
		add_executable (${APP} examples/${APP}.cpp)
		target_link_libraries(${APP} ${PROJECT_NAME} ${OS_LIBS})
		#include_directories(${APP} include)
	endforeach()

	# Networking applications
	set(EXAMPLE_NETWORKING_APPS
		test_client
		test_client_jsonb
		test_server
		test_server_jsonb
		test_server_daemon_jsonb
	)
	foreach (APP ${EXAMPLE_NETWORKING_APPS})
		add_executable(networking_${APP} examples/networking/${APP}.cpp)
		target_link_libraries(networking_${APP} ${PROJECT_NAME} ${OS_LIBS})
	endforeach ()
endif ()

#
# Tests
#

if (VEPHOR_BUILD_TESTING)
    if(GTest_FOUND)
        add_subdirectory(test/unit)
    else()
        message(WARNING "GTest not found, tests will not be built.")
    endif()
endif()
