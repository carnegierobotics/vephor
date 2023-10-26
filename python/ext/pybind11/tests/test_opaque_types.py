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

import pytest

from pybind11_tests import ConstructorStats, UserType
from pybind11_tests import opaque_types as m


def test_string_list():
    lst = m.StringList()
    lst.push_back("Element 1")
    lst.push_back("Element 2")
    assert m.print_opaque_list(lst) == "Opaque list: [Element 1, Element 2]"
    assert lst.back() == "Element 2"

    for i, k in enumerate(lst, start=1):
        assert k == f"Element {i}"
    lst.pop_back()
    assert m.print_opaque_list(lst) == "Opaque list: [Element 1]"

    cvp = m.ClassWithSTLVecProperty()
    assert m.print_opaque_list(cvp.stringList) == "Opaque list: []"

    cvp.stringList = lst
    cvp.stringList.push_back("Element 3")
    assert m.print_opaque_list(cvp.stringList) == "Opaque list: [Element 1, Element 3]"


def test_pointers(msg):
    living_before = ConstructorStats.get(UserType).alive()
    assert m.get_void_ptr_value(m.return_void_ptr()) == 0x1234
    assert m.get_void_ptr_value(UserType())  # Should also work for other C++ types
    assert ConstructorStats.get(UserType).alive() == living_before

    with pytest.raises(TypeError) as excinfo:
        m.get_void_ptr_value([1, 2, 3])  # This should not work
    assert (
        msg(excinfo.value)
        == """
        get_void_ptr_value(): incompatible function arguments. The following argument types are supported:
            1. (arg0: capsule) -> int

        Invoked with: [1, 2, 3]
    """
    )

    assert m.return_null_str() is None
    assert m.get_null_str_value(m.return_null_str()) is not None

    ptr = m.return_unique_ptr()
    assert "StringList" in repr(ptr)
    assert m.print_opaque_list(ptr) == "Opaque list: [some value]"


def test_unions():
    int_float_union = m.IntFloat()
    int_float_union.i = 42
    assert int_float_union.i == 42
    int_float_union.f = 3.0
    assert int_float_union.f == 3.0
