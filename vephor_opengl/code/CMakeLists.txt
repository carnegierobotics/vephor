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

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

if (WIN32)
	set(GRAPHICS_LIBS opengl32 glfw3 glew32 Ws2_32)
else()
	set(GRAPHICS_LIBS ${OpenGL_LIBRARIES} GL glfw3 ${GLEW_LIBRARIES} dl X11 pthread)
endif()

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
								 VERSION ${PROJECT_VERSION}
								 COMPATIBILITY AnyNewerVersion)

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
install(DIRECTORY include DESTINATION "${CMAKE_INSTALL_PREFIX}")

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION
  ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

INSTALL(TARGETS vephor_show DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
#INSTALL(TARGETS vephor_opengl DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
INSTALL(DIRECTORY include DESTINATION "${CMAKE_INSTALL_PREFIX}")