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

#!/usr/bin/env python3

import re

import ghapi.all
from rich import print
from rich.syntax import Syntax

ENTRY = re.compile(
    r"""
    Suggested \s changelog \s entry:
    .*
    ```rst
    \s*
    (.*?)
    \s*
    ```
""",
    re.DOTALL | re.VERBOSE,
)

print()


api = ghapi.all.GhApi(owner="pybind", repo="pybind11")

issues_pages = ghapi.page.paged(
    api.issues.list_for_repo, labels="needs changelog", state="closed"
)
issues = (issue for page in issues_pages for issue in page)
missing = []

for issue in issues:
    changelog = ENTRY.findall(issue.body or "")
    if not changelog or not changelog[0]:
        missing.append(issue)
    else:
        (msg,) = changelog
        if not msg.startswith("* "):
            msg = "* " + msg
        if not msg.endswith("."):
            msg += "."

        msg += f"\n  `#{issue.number} <{issue.html_url}>`_"

        print(Syntax(msg, "rst", theme="ansi_light", word_wrap=True))
        print()

if missing:
    print()
    print("[blue]" + "-" * 30)
    print()

    for issue in missing:
        print(f"[red bold]Missing:[/red bold][red] {issue.title}")
        print(f"[red]  {issue.html_url}\n")

    print("[bold]Template:\n")
    msg = "## Suggested changelog entry:\n\n```rst\n\n```"
    print(Syntax(msg, "md", theme="ansi_light"))

print()
