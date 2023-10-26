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

"""Simple script for rebuilding .codespell-ignore-lines

Usage:

cat < /dev/null > .codespell-ignore-lines
pre-commit run --all-files codespell >& /tmp/codespell_errors.txt
python3 tools/codespell_ignore_lines_from_errors.py /tmp/codespell_errors.txt > .codespell-ignore-lines

git diff to review changes, then commit, push.
"""

import sys
from typing import List


def run(args: List[str]) -> None:
    assert len(args) == 1, "codespell_errors.txt"
    cache = {}
    done = set()
    for line in sorted(open(args[0]).read().splitlines()):
        i = line.find(" ==> ")
        if i > 0:
            flds = line[:i].split(":")
            if len(flds) >= 2:
                filename, line_num = flds[:2]
                if filename not in cache:
                    cache[filename] = open(filename).read().splitlines()
                supp = cache[filename][int(line_num) - 1]
                if supp not in done:
                    print(supp)
                    done.add(supp)


if __name__ == "__main__":
    run(args=sys.argv[1:])
