cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

project(vephor_opengl)

add_subdirectory(code)

INSTALL(DIRECTORY assets DESTINATION "${CMAKE_INSTALL_PREFIX}/share/vephor/")
