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

import gc
import weakref

import pytest

import env  # noqa: F401
from pybind11_tests import custom_type_setup as m


@pytest.fixture
def gc_tester():
    """Tests that an object is garbage collected.

    Assumes that any unreferenced objects are fully collected after calling
    `gc.collect()`.  That is true on CPython, but does not appear to reliably
    hold on PyPy.
    """

    weak_refs = []

    def add_ref(obj):
        # PyPy does not support `gc.is_tracked`.
        if hasattr(gc, "is_tracked"):
            assert gc.is_tracked(obj)
        weak_refs.append(weakref.ref(obj))

    yield add_ref

    gc.collect()
    for ref in weak_refs:
        assert ref() is None


# PyPy does not seem to reliably garbage collect.
@pytest.mark.skipif("env.PYPY")
def test_self_cycle(gc_tester):
    obj = m.OwnsPythonObjects()
    obj.value = obj
    gc_tester(obj)


# PyPy does not seem to reliably garbage collect.
@pytest.mark.skipif("env.PYPY")
def test_indirect_cycle(gc_tester):
    obj = m.OwnsPythonObjects()
    obj_list = [obj]
    obj.value = obj_list
    gc_tester(obj)
