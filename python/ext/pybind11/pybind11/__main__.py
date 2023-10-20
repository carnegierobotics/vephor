#
# Copyright 2023
# Carnegie Robotics, LLC
# 4501 Hatfield Street, Pittsburgh, PA 15201
# https://www.carnegierobotics.com
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Carnegie Robotics, LLC nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL CARNEGIE ROBOTICS, LLC BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

# pylint: disable=missing-function-docstring

import argparse
import sys
import sysconfig

from .commands import get_cmake_dir, get_include, get_pkgconfig_dir


def print_includes() -> None:
    dirs = [
        sysconfig.get_path("include"),
        sysconfig.get_path("platinclude"),
        get_include(),
    ]

    # Make unique but preserve order
    unique_dirs = []
    for d in dirs:
        if d and d not in unique_dirs:
            unique_dirs.append(d)

    print(" ".join("-I" + d for d in unique_dirs))


def main() -> None:

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--includes",
        action="store_true",
        help="Include flags for both pybind11 and Python headers.",
    )
    parser.add_argument(
        "--cmakedir",
        action="store_true",
        help="Print the CMake module directory, ideal for setting -Dpybind11_ROOT in CMake.",
    )
    parser.add_argument(
        "--pkgconfigdir",
        action="store_true",
        help="Print the pkgconfig directory, ideal for setting $PKG_CONFIG_PATH.",
    )
    args = parser.parse_args()
    if not sys.argv[1:]:
        parser.print_help()
    if args.includes:
        print_includes()
    if args.cmakedir:
        print(get_cmake_dir())
    if args.pkgconfigdir:
        print(get_pkgconfig_dir())


if __name__ == "__main__":
    main()
