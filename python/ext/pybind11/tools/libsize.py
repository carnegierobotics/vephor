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

import os
import sys

# Internal build script for generating debugging test .so size.
# Usage:
#     python libsize.py file.so save.txt -- displays the size of file.so and, if save.txt exists, compares it to the
#                                           size in it, then overwrites save.txt with the new size for future runs.

if len(sys.argv) != 3:
    sys.exit("Invalid arguments: usage: python libsize.py file.so save.txt")

lib = sys.argv[1]
save = sys.argv[2]

if not os.path.exists(lib):
    sys.exit(f"Error: requested file ({lib}) does not exist")

libsize = os.path.getsize(lib)

print("------", os.path.basename(lib), "file size:", libsize, end="")

if os.path.exists(save):
    with open(save) as sf:
        oldsize = int(sf.readline())

    if oldsize > 0:
        change = libsize - oldsize
        if change == 0:
            print(" (no change)")
        else:
            print(f" (change of {change:+} bytes = {change / oldsize:+.2%})")
else:
    print()

with open(save, "w") as sf:
    sf.write(str(libsize))
