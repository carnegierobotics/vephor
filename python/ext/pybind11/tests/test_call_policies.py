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

import env  # noqa: F401
from pybind11_tests import ConstructorStats
from pybind11_tests import call_policies as m


@pytest.mark.xfail("env.PYPY", reason="sometimes comes out 1 off on PyPy", strict=False)
def test_keep_alive_argument(capture):
    n_inst = ConstructorStats.detail_reg_inst()
    with capture:
        p = m.Parent()
    assert capture == "Allocating parent."
    with capture:
        p.addChild(m.Child())
        assert ConstructorStats.detail_reg_inst() == n_inst + 1
    assert (
        capture
        == """
        Allocating child.
        Releasing child.
    """
    )
    with capture:
        del p
        assert ConstructorStats.detail_reg_inst() == n_inst
    assert capture == "Releasing parent."

    with capture:
        p = m.Parent()
    assert capture == "Allocating parent."
    with capture:
        p.addChildKeepAlive(m.Child())
        assert ConstructorStats.detail_reg_inst() == n_inst + 2
    assert capture == "Allocating child."
    with capture:
        del p
        assert ConstructorStats.detail_reg_inst() == n_inst
    assert (
        capture
        == """
        Releasing parent.
        Releasing child.
    """
    )

    p = m.Parent()
    c = m.Child()
    assert ConstructorStats.detail_reg_inst() == n_inst + 2
    m.free_function(p, c)
    del c
    assert ConstructorStats.detail_reg_inst() == n_inst + 2
    del p
    assert ConstructorStats.detail_reg_inst() == n_inst

    with pytest.raises(RuntimeError) as excinfo:
        m.invalid_arg_index()
    assert str(excinfo.value) == "Could not activate keep_alive!"


def test_keep_alive_return_value(capture):
    n_inst = ConstructorStats.detail_reg_inst()
    with capture:
        p = m.Parent()
    assert capture == "Allocating parent."
    with capture:
        p.returnChild()
        assert ConstructorStats.detail_reg_inst() == n_inst + 1
    assert (
        capture
        == """
        Allocating child.
        Releasing child.
    """
    )
    with capture:
        del p
        assert ConstructorStats.detail_reg_inst() == n_inst
    assert capture == "Releasing parent."

    with capture:
        p = m.Parent()
    assert capture == "Allocating parent."
    with capture:
        p.returnChildKeepAlive()
        assert ConstructorStats.detail_reg_inst() == n_inst + 2
    assert capture == "Allocating child."
    with capture:
        del p
        assert ConstructorStats.detail_reg_inst() == n_inst
    assert (
        capture
        == """
        Releasing parent.
        Releasing child.
    """
    )

    p = m.Parent()
    assert ConstructorStats.detail_reg_inst() == n_inst + 1
    with capture:
        m.Parent.staticFunction(p)
        assert ConstructorStats.detail_reg_inst() == n_inst + 2
    assert capture == "Allocating child."
    with capture:
        del p
        assert ConstructorStats.detail_reg_inst() == n_inst
    assert (
        capture
        == """
        Releasing parent.
        Releasing child.
    """
    )


# https://foss.heptapod.net/pypy/pypy/-/issues/2447
@pytest.mark.xfail("env.PYPY", reason="_PyObject_GetDictPtr is unimplemented")
def test_alive_gc(capture):
    n_inst = ConstructorStats.detail_reg_inst()
    p = m.ParentGC()
    p.addChildKeepAlive(m.Child())
    assert ConstructorStats.detail_reg_inst() == n_inst + 2
    lst = [p]
    lst.append(lst)  # creates a circular reference
    with capture:
        del p, lst
        assert ConstructorStats.detail_reg_inst() == n_inst
    assert (
        capture
        == """
        Releasing parent.
        Releasing child.
    """
    )


def test_alive_gc_derived(capture):
    class Derived(m.Parent):
        pass

    n_inst = ConstructorStats.detail_reg_inst()
    p = Derived()
    p.addChildKeepAlive(m.Child())
    assert ConstructorStats.detail_reg_inst() == n_inst + 2
    lst = [p]
    lst.append(lst)  # creates a circular reference
    with capture:
        del p, lst
        assert ConstructorStats.detail_reg_inst() == n_inst
    assert (
        capture
        == """
        Releasing parent.
        Releasing child.
    """
    )


def test_alive_gc_multi_derived(capture):
    class Derived(m.Parent, m.Child):
        def __init__(self):
            m.Parent.__init__(self)
            m.Child.__init__(self)

    n_inst = ConstructorStats.detail_reg_inst()
    p = Derived()
    p.addChildKeepAlive(m.Child())
    # +3 rather than +2 because Derived corresponds to two registered instances
    assert ConstructorStats.detail_reg_inst() == n_inst + 3
    lst = [p]
    lst.append(lst)  # creates a circular reference
    with capture:
        del p, lst
        assert ConstructorStats.detail_reg_inst() == n_inst
    assert (
        capture
        == """
        Releasing parent.
        Releasing child.
        Releasing child.
    """
    )


def test_return_none(capture):
    n_inst = ConstructorStats.detail_reg_inst()
    with capture:
        p = m.Parent()
    assert capture == "Allocating parent."
    with capture:
        p.returnNullChildKeepAliveChild()
        assert ConstructorStats.detail_reg_inst() == n_inst + 1
    assert capture == ""
    with capture:
        del p
        assert ConstructorStats.detail_reg_inst() == n_inst
    assert capture == "Releasing parent."

    with capture:
        p = m.Parent()
    assert capture == "Allocating parent."
    with capture:
        p.returnNullChildKeepAliveParent()
        assert ConstructorStats.detail_reg_inst() == n_inst + 1
    assert capture == ""
    with capture:
        del p
        assert ConstructorStats.detail_reg_inst() == n_inst
    assert capture == "Releasing parent."


def test_keep_alive_constructor(capture):
    n_inst = ConstructorStats.detail_reg_inst()

    with capture:
        p = m.Parent(m.Child())
        assert ConstructorStats.detail_reg_inst() == n_inst + 2
    assert (
        capture
        == """
        Allocating child.
        Allocating parent.
    """
    )
    with capture:
        del p
        assert ConstructorStats.detail_reg_inst() == n_inst
    assert (
        capture
        == """
        Releasing parent.
        Releasing child.
    """
    )


def test_call_guard():
    assert m.unguarded_call() == "unguarded"
    assert m.guarded_call() == "guarded"

    assert m.multiple_guards_correct_order() == "guarded & guarded"
    assert m.multiple_guards_wrong_order() == "unguarded & guarded"

    if hasattr(m, "with_gil"):
        assert m.with_gil() == "GIL held"
        assert m.without_gil() == "GIL released"
