cmake_minimum_required(VERSION 3.15...3.25)

set(CMAKE_CXX_STANDARD 17)

project(${SKBUILD_PROJECT_NAME} VERSION ${SKBUILD_PROJECT_VERSION})

add_subdirectory(ext/pybind11)

find_package(Eigen3 3.3 REQUIRED)
find_package(manif REQUIRED)

add_compile_options(-isystem ${EIGEN3_INCLUDE_DIR})
add_compile_options(-isystem ${MANIF_INCLUDE_DIR})

pybind11_add_module(_core MODULE vephor/main.cpp)
target_compile_definitions(_core PRIVATE VEPHOR_ASSET_INSTALL_PREFIX="/opt")
target_compile_definitions(_core PRIVATE VERSION_INFO=${PROJECT_VERSION})
if (WIN32)
	target_link_libraries(_core PRIVATE Ws2_32)
endif()
INSTALL(TARGETS _core DESTINATION vephor)
