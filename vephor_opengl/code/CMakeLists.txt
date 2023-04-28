cmake_minimum_required(VERSION 3.16)

project(vephor_opengl)

if (TARGET vephor)
	message("vephor target found, not calling find_package")
else()
	find_package(vephor CONFIG REQUIRED)
endif()

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

if (WIN32)
	set(GRAPHICS_LIBS opengl32 glfw3 glew32 Ws2_32)
else()
	set(GRAPHICS_LIBS ${OpenGL_LIBRARIES} GL glfw3 ${GLEW_LIBRARIES} dl X11 pthread)
endif()

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
	two_windows
)

add_library(vephor_opengl
	src/ogl/window.cpp 
	src/ogl/mesh.cpp 
	src/ogl/lines.cpp
	src/ogl/thick_lines.cpp	
	src/ogl/io.cpp
	src/ogl/light.cpp
	src/ogl/text.cpp
	src/ogl/sprite.cpp
	src/ogl/particle.cpp
	src/ogl/points.cpp
	src/ogl/instanced_points.cpp
	src/ogl/background.cpp
	src/mcubes.cpp
	src/verlet.cpp
)
target_link_libraries(vephor_opengl ${GRAPHICS_LIBS} vephor)

foreach (APP ${EXAMPLE_APPS})
	add_executable (${APP} examples/${APP}.cpp)
	include_directories(${APP} include)
	target_link_libraries(${APP} vephor_opengl)
endforeach()

add_executable (vephor_show apps/show/show.cpp apps/show/plot_camera.cpp apps/show/trackball_camera.cpp apps/show/show_record_window.cpp)
include_directories(vephor_show include)
target_link_libraries(vephor_show vephor_opengl)

INSTALL(TARGETS vephor_show DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
INSTALL(TARGETS vephor_opengl DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
INSTALL(DIRECTORY include DESTINATION "${CMAKE_INSTALL_PREFIX}")