cmake_minimum_required(VERSION 3.16)

project(vephor VERSION 0.1)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# Some cmake sections adapted from:
#	https://stackoverflow.com/questions/47718485/install-and-export-interface-only-library-cmake
#	https://dominikberner.ch/cmake-interface-lib/

add_library(vephor INTERFACE)
target_include_directories(vephor INTERFACE 
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
	
include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
								 VERSION ${PROJECT_VERSION}
								 COMPATIBILITY AnyNewerVersion)

set(EXAMPLE_APPS
	test_show
	mini_test_show
	test_plot
	test_imshow
	test_plot_render
	test_show_two_windows
	test_client
	test_server
	test_client_jsonb
	test_server_jsonb
	test_server_daemon_jsonb
	produce_sprites_with_normals
)

foreach (APP ${EXAMPLE_APPS})
	add_executable (${APP} examples/${APP}.cpp)
	target_link_libraries(${APP} vephor)
	#include_directories(${APP} include)
	
	if (WIN32)
		target_link_libraries(${APP} Ws2_32)
	else()
		target_link_libraries(${APP} pthread)
	endif()
endforeach()

set(BUILD_UNIT_TESTS ON CACHE BOOL "Build unit tests")

if (BUILD_UNIT_TESTS)
    find_package(GTest)
    if(GTEST_FOUND)
        add_subdirectory(test/unit)
    else(GTEST_FOUND)
        message(WARNING "GTest not found, tests will not be built.")
    endif()
endif()


install(TARGETS vephor
    EXPORT vehpor_targets
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    RUNTIME DESTINATION bin COMPONENT Runtime
    PUBLIC_HEADER DESTINATION include COMPONENT Development
    BUNDLE DESTINATION bin COMPONENT Runtime)

install(EXPORT vehpor_targets DESTINATION lib/cmake/vephor)
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        DESTINATION lib/cmake/vephor)
install(DIRECTORY include DESTINATION "${CMAKE_INSTALL_PREFIX}")

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION
  ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)
