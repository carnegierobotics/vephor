#
# Copyright 2025
# Carnegie Robotics, LLC
# 4501 Hatfield Street, Pittsburgh, PA 15201
# https://www.carnegierobotics.com
#
# This source code is licensed under the Apache License, Version 2.0
# found in the LICENSE file in the root directory of this source tree.
#

cmake_minimum_required(VERSION 3.15...3.29)

set(CMAKE_CXX_STANDARD 17)

project(${SKBUILD_PROJECT_NAME} VERSION ${SKBUILD_PROJECT_VERSION})

add_subdirectory(ext/pybind11)

#
# Python module
#

set(PACKAGE_NAME vephor)
set(MODULE_NAME _core)

pybind11_add_module(${MODULE_NAME}
	MODULE vephor/main.cpp
)
target_compile_definitions(${MODULE_NAME}
	PRIVATE
		VERSION_INFO=${PROJECT_VERSION}
)
target_link_libraries(${MODULE_NAME}
	PRIVATE
		vephor
)

if(VEPHOR_BUILD_OPENGL)
	target_link_libraries(${MODULE_NAME}
		PRIVATE
			vephor_opengl
	)
	target_compile_definitions(${MODULE_NAME}
		PRIVATE
		INCLUDE_OPENGL_BINDINGS
	)
endif()

set_target_properties(${MODULE_NAME} PROPERTIES
    INSTALL_RPATH "$ORIGIN/lib"
)

if (WIN32)
	target_link_libraries(${MODULE_NAME}
		PRIVATE
			Ws2_32
	)
endif()
install(
	TARGETS ${MODULE_NAME}
	DESTINATION ${PACKAGE_NAME}
)

# Configure and install an __init__.py
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME})
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_NAME}/__init__.py.in
	${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/__init__.py
	@ONLY
)
install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/__init__.py
	DESTINATION ${PACKAGE_NAME}
)
