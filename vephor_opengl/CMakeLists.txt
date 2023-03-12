cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

project(vephor_opengl)

add_subdirectory(code)

if (WIN32)
	INSTALL(DIRECTORY assets DESTINATION "c:/Users/Public/vephor/")
else()
	INSTALL(DIRECTORY assets DESTINATION "/opt/vephor/")
endif()