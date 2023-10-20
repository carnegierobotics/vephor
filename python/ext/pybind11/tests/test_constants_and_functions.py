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

m = pytest.importorskip("pybind11_tests.constants_and_functions")


def test_constants():
    assert m.some_constant == 14


def test_function_overloading():
    assert m.test_function() == "test_function()"
    assert m.test_function(7) == "test_function(7)"
    assert m.test_function(m.MyEnum.EFirstEntry) == "test_function(enum=1)"
    assert m.test_function(m.MyEnum.ESecondEntry) == "test_function(enum=2)"

    assert m.test_function() == "test_function()"
    assert m.test_function("abcd") == "test_function(char *)"
    assert m.test_function(1, 1.0) == "test_function(int, float)"
    assert m.test_function(1, 1.0) == "test_function(int, float)"
    assert m.test_function(2.0, 2) == "test_function(float, int)"


def test_bytes():
    assert m.print_bytes(m.return_bytes()) == "bytes[1 0 2 0]"


def test_exception_specifiers():
    c = m.C()
    assert c.m1(2) == 1
    assert c.m2(3) == 1
    assert c.m3(5) == 2
    assert c.m4(7) == 3
    assert c.m5(10) == 5
    assert c.m6(14) == 8
    assert c.m7(20) == 13
    assert c.m8(29) == 21

    assert m.f1(33) == 34
    assert m.f2(53) == 55
    assert m.f3(86) == 89
    assert m.f4(140) == 144


def test_function_record_leaks():
    class RaisingRepr:
        def __repr__(self):
            raise RuntimeError("Surprise!")

    with pytest.raises(RuntimeError):
        m.register_large_capture_with_invalid_arguments(m)
    with pytest.raises(RuntimeError):
        m.register_with_raising_repr(m, RaisingRepr())
