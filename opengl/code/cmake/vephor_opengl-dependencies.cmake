#
# Copyright 2025
# Carnegie Robotics, LLC
# 4501 Hatfield Street, Pittsburgh, PA 15201
# https://www.carnegierobotics.com
#
# This source code is licensed under the Apache License, Version 2.0
# found in the LICENSE file in the root directory of this source tree.
#

#
# Required dependencies
#

if (TARGET vephor)
    message(STATUS "vephor target found, not calling find_package")
else ()
    find_package(vephor CONFIG REQUIRED)
endif ()

# Conda environments often wipe system library paths.
# Append standard Linux library paths so we can find system OpenGL.
if(DEFINED ENV{CONDA_PREFIX})
    list(APPEND CMAKE_LIBRARY_PATH "/usr/lib/x86_64-linux-gnu" "/usr/lib/aarch64-linux-gnu" "/usr/lib")
endif()

find_package(OpenGL REQUIRED)

find_package(GLEW REQUIRED)

find_package(glfw3 REQUIRED)
