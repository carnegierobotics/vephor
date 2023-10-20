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
import subprocess
import sys
from textwrap import dedent

import pytest

DIR = os.path.abspath(os.path.dirname(__file__))
MAIN_DIR = os.path.dirname(os.path.dirname(DIR))
WIN = sys.platform.startswith("win32") or sys.platform.startswith("cygwin")


@pytest.mark.parametrize("parallel", [False, True])
@pytest.mark.parametrize("std", [11, 0])
def test_simple_setup_py(monkeypatch, tmpdir, parallel, std):
    monkeypatch.chdir(tmpdir)
    monkeypatch.syspath_prepend(MAIN_DIR)

    (tmpdir / "setup.py").write_text(
        dedent(
            f"""\
            import sys
            sys.path.append({MAIN_DIR!r})

            from setuptools import setup, Extension
            from pybind11.setup_helpers import build_ext, Pybind11Extension

            std = {std}

            ext_modules = [
                Pybind11Extension(
                    "simple_setup",
                    sorted(["main.cpp"]),
                    cxx_std=std,
                ),
            ]

            cmdclass = dict()
            if std == 0:
                cmdclass["build_ext"] = build_ext


            parallel = {parallel}
            if parallel:
                from pybind11.setup_helpers import ParallelCompile
                ParallelCompile().install()

            setup(
                name="simple_setup_package",
                cmdclass=cmdclass,
                ext_modules=ext_modules,
            )
            """
        ),
        encoding="ascii",
    )

    (tmpdir / "main.cpp").write_text(
        dedent(
            """\
            #include <pybind11/pybind11.h>

            int f(int x) {
                return x * 3;
            }
            PYBIND11_MODULE(simple_setup, m) {
                m.def("f", &f);
            }
            """
        ),
        encoding="ascii",
    )

    out = subprocess.check_output(
        [sys.executable, "setup.py", "build_ext", "--inplace"],
    )
    if not WIN:
        assert b"-g0" in out
    out = subprocess.check_output(
        [sys.executable, "setup.py", "build_ext", "--inplace", "--force"],
        env=dict(os.environ, CFLAGS="-g"),
    )
    if not WIN:
        assert b"-g0" not in out

    # Debug helper printout, normally hidden
    print(out)
    for item in tmpdir.listdir():
        print(item.basename)

    assert (
        len([f for f in tmpdir.listdir() if f.basename.startswith("simple_setup")]) == 1
    )
    assert len(list(tmpdir.listdir())) == 4  # two files + output + build_dir

    (tmpdir / "test.py").write_text(
        dedent(
            """\
            import simple_setup
            assert simple_setup.f(3) == 9
            """
        ),
        encoding="ascii",
    )

    subprocess.check_call(
        [sys.executable, "test.py"], stdout=sys.stdout, stderr=sys.stderr
    )


def test_intree_extensions(monkeypatch, tmpdir):
    monkeypatch.syspath_prepend(MAIN_DIR)

    from pybind11.setup_helpers import intree_extensions

    monkeypatch.chdir(tmpdir)
    root = tmpdir
    root.ensure_dir()
    subdir = root / "dir"
    subdir.ensure_dir()
    src = subdir / "ext.cpp"
    src.ensure()
    relpath = src.relto(tmpdir)
    (ext,) = intree_extensions([relpath])
    assert ext.name == "ext"
    subdir.ensure("__init__.py")
    (ext,) = intree_extensions([relpath])
    assert ext.name == "dir.ext"


def test_intree_extensions_package_dir(monkeypatch, tmpdir):
    monkeypatch.syspath_prepend(MAIN_DIR)

    from pybind11.setup_helpers import intree_extensions

    monkeypatch.chdir(tmpdir)
    root = tmpdir / "src"
    root.ensure_dir()
    subdir = root / "dir"
    subdir.ensure_dir()
    src = subdir / "ext.cpp"
    src.ensure()
    (ext,) = intree_extensions([src.relto(tmpdir)], package_dir={"": "src"})
    assert ext.name == "dir.ext"
    (ext,) = intree_extensions([src.relto(tmpdir)], package_dir={"foo": "src"})
    assert ext.name == "foo.dir.ext"
    subdir.ensure("__init__.py")
    (ext,) = intree_extensions([src.relto(tmpdir)], package_dir={"": "src"})
    assert ext.name == "dir.ext"
    (ext,) = intree_extensions([src.relto(tmpdir)], package_dir={"foo": "src"})
    assert ext.name == "foo.dir.ext"
