#
# Copyright 2025
# Carnegie Robotics, LLC
# 4501 Hatfield Street, Pittsburgh, PA 15201
# https://www.carnegierobotics.com
#
# This source code is licensed under the Apache License, Version 2.0
# found in the LICENSE file in the root directory of this source tree.
#

cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

project(vephor VERSION 0.1)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

include(GNUInstallDirs)

add_subdirectory(code)

# Allow apps in build dir to find assets
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/share/vephor)

install(
    DIRECTORY assets
    DESTINATION ${INSTALL_DATADIR}/vephor
)
