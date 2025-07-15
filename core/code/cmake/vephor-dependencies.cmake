#
# Copyright 2025
# Carnegie Robotics, LLC
# 4501 Hatfield Street, Pittsburgh, PA 15201
# https://www.carnegierobotics.com
#
# This source code is licensed under the Apache License, Version 2.0
# found in the LICENSE file in the root directory of this source tree.
#

include(FetchContent)

#
# Required dependencies
#

find_package(Eigen3 3.3 QUIET)
if (NOT Eigen3_FOUND)
    FetchContent_Declare(
            Eigen
            GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
            GIT_TAG 3.4
    )
    FetchContent_MakeAvailable(Eigen)
endif ()


find_package(manif CONFIG QUIET)
if (NOT manif_FOUND)
    FetchContent_Declare(
            manif
            GIT_REPOSITORY https://github.com/artivis/manif.git
    )
    FetchContent_MakeAvailable(manif)
endif ()

#
# Optional dependencies
#

find_package(nlohmann_json QUIET)

if (VEPHOR_BUILD_TESTING)
    find_package(GTest QUIET)
endif ()
