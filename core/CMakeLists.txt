cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

project(vephor)

add_subdirectory(code)

# Allow apps in build dir to find assets
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/share/vephor)

INSTALL(DIRECTORY assets DESTINATION share/vephor)
