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

# Make CMake scripts available
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(CMakePackageConfigHelpers)

#
# vephor_opengl
#

include(${PROJECT_NAME}-dependencies)

if (WIN32)
	set(GRAPHICS_LIBS opengl32 glfw glew32 Ws2_32)
else()
	set(GRAPHICS_LIBS ${OpenGL_LIBRARIES} GL glfw ${GLEW_LIBRARIES} dl X11 pthread)
endif()

add_library(${PROJECT_NAME} SHARED
	src/ogl/window.cpp 
	src/ogl/draw/background.cpp	
	src/ogl/draw/instanced_points.cpp
	src/ogl/draw/light.cpp
	src/ogl/draw/lines.cpp
	src/ogl/draw/material.cpp 
	src/ogl/draw/mesh.cpp 
	src/ogl/draw/particle.cpp
	src/ogl/draw/points.cpp
	src/ogl/draw/sprite.cpp
	src/ogl/draw/text.cpp
	src/ogl/draw/thick_lines.cpp
	src/ogl/camera/plot_camera.cpp
	src/ogl/camera/plot3d_camera.cpp
	src/ogl/camera/trackball_camera.cpp
	src/ogl/camera/trajectory_camera.cpp
	src/ogl/io.cpp
	src/verlet.cpp
)
target_link_libraries(${PROJECT_NAME}
	${GRAPHICS_LIBS}
	vephor
)
target_include_directories(${PROJECT_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    	$<INSTALL_INTERFACE:${INSTALL_INCLUDEDIR}>
)
set_property(TARGET ${PROJECT_NAME}
    APPEND PROPERTY PUBLIC_HEADER
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
)  # TODO: Should public headers be specified granularly?

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
# vephor_show
#

set(VEPHOR_SHOW_TARGET vephor_show)

add_executable(${VEPHOR_SHOW_TARGET}
	${CMAKE_CURRENT_SOURCE_DIR}/apps/show/show.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/apps/show/show_record_window.cpp
)
include_directories(${VEPHOR_SHOW_TARGET} ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${VEPHOR_SHOW_TARGET} ${PROJECT_NAME})

# Update RPATH s that executable can find the shared objects it is linked with
# TODO: Make this robust to cases where INSTALL_BINDIR AND INSTALL_LIBDIR are not adjacent.
set_target_properties(${VEPHOR_SHOW_TARGET} PROPERTIES
	INSTALL_RPATH "$ORIGIN/../${INSTALL_LIBDIR}"
)

install(
	TARGETS ${VEPHOR_SHOW_TARGET}
	DESTINATION ${INSTALL_BINDIR}
)

#
# Example applications
#

if (VEPHOR_BUILD_PYTHON_BINDINGS)
	message(STATUS "Not building OpenGL examples due to python bindings.")
else (VEPHOR_BUILD_PYTHON_BINDINGS)
	set(EXAMPLE_APPS
		background_test
		basic
		camera_test
		just_instanced_points
		just_sphere
		marching_cubes
		material_test
		physics
		physics_constraints
		physics_height_map
		physics_solid
		screenshot_test
		shadows_test
		skybox_test
		sprites
		thick_lines_test
		trackball_camera_test
		two_windows
	)

	foreach (APP ${EXAMPLE_APPS})
		add_executable (${APP} ${CMAKE_CURRENT_SOURCE_DIR}/examples/${APP}.cpp)
		include_directories(${APP} ${CMAKE_CURRENT_SOURCE_DIR}/include)
		target_link_libraries(${APP} ${PROJECT_NAME})
	endforeach()
endif (VEPHOR_BUILD_PYTHON_BINDINGS)
