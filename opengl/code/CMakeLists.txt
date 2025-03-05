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

project(vephor_opengl VERSION 0.1)

if (TARGET vephor)
	message("vephor target found, not calling find_package")
else()
	find_package(vephor CONFIG REQUIRED)
endif()

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

if (WIN32)
	set(GRAPHICS_LIBS opengl32 glfw glew32 Ws2_32)
else()
	set(GRAPHICS_LIBS ${OpenGL_LIBRARIES} GL glfw ${GLEW_LIBRARIES} dl X11 pthread)
endif()

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
								 VERSION ${PROJECT_VERSION}
								 COMPATIBILITY AnyNewerVersion)

add_library(vephor_opengl SHARED
	src/ogl/window.cpp 
	src/ogl/draw/background.cpp	
	src/ogl/draw/instanced_points.cpp
	src/ogl/draw/light.cpp
	src/ogl/draw/lines.cpp
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
target_link_libraries(vephor_opengl ${GRAPHICS_LIBS} vephor)

if (VEPHOR_BUILD_PYTHON_BINDINGS)
	message("Not building OpenGL examples due to python bindings.")
else (VEPHOR_BUILD_PYTHON_BINDINGS)
	set(EXAMPLE_APPS
		background_test
		basic
		camera_test
		marching_cubes
		physics
		physics_constraints
		physics_height_map
		physics_solid
		screenshot_test
		sprites
		thick_lines_test
		trackball_camera_test
		two_windows
	)

	foreach (APP ${EXAMPLE_APPS})
		add_executable (${APP} examples/${APP}.cpp)
		include_directories(${APP} include)
		target_link_libraries(${APP} vephor_opengl)
	endforeach()
endif (VEPHOR_BUILD_PYTHON_BINDINGS)

add_executable (vephor_show 
	apps/show/show.cpp 
	apps/show/show_record_window.cpp)
include_directories(vephor_show include)
target_link_libraries(vephor_show vephor_opengl)

install(TARGETS vephor_opengl
    EXPORT vephor_opengl_targets
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    RUNTIME DESTINATION bin COMPONENT Runtime
    PUBLIC_HEADER DESTINATION include COMPONENT Development
    BUNDLE DESTINATION bin COMPONENT Runtime)

install(EXPORT vephor_opengl_targets DESTINATION lib/cmake/vephor_opengl)
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        DESTINATION lib/cmake/vephor_opengl)
install(DIRECTORY include DESTINATION .)

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION
  ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

INSTALL(TARGETS vephor_show DESTINATION bin)