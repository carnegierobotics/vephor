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

from pybind11_tests import docstring_options as m


def test_docstring_options():
    # options.disable_function_signatures()
    assert not m.test_function1.__doc__

    assert m.test_function2.__doc__ == "A custom docstring"

    # docstring specified on just the first overload definition:
    assert m.test_overloaded1.__doc__ == "Overload docstring"

    # docstring on both overloads:
    assert m.test_overloaded2.__doc__ == "overload docstring 1\noverload docstring 2"

    # docstring on only second overload:
    assert m.test_overloaded3.__doc__ == "Overload docstr"

    # options.enable_function_signatures()
    assert m.test_function3.__doc__.startswith("test_function3(a: int, b: int) -> None")

    assert m.test_function4.__doc__.startswith("test_function4(a: int, b: int) -> None")
    assert m.test_function4.__doc__.endswith("A custom docstring\n")

    # options.disable_function_signatures()
    # options.disable_user_defined_docstrings()
    assert not m.test_function5.__doc__

    # nested options.enable_user_defined_docstrings()
    assert m.test_function6.__doc__ == "A custom docstring"

    # RAII destructor
    assert m.test_function7.__doc__.startswith("test_function7(a: int, b: int) -> None")
    assert m.test_function7.__doc__.endswith("A custom docstring\n")

    # when all options are disabled, no docstring (instead of an empty one) should be generated
    assert m.test_function8.__doc__ is None

    # Suppression of user-defined docstrings for non-function objects
    assert not m.DocstringTestFoo.__doc__
    assert not m.DocstringTestFoo.value_prop.__doc__

    # Check existig behaviour of enum docstings
    assert (
        m.DocstringTestEnum1.__doc__
        == "Enum docstring\n\nMembers:\n\n  Member1\n\n  Member2"
    )

    # options.enable_enum_members_docstring()
    assert (
        m.DocstringTestEnum2.__doc__
        == "Enum docstring\n\nMembers:\n\n  Member1\n\n  Member2"
    )

    # options.disable_enum_members_docstring()
    assert m.DocstringTestEnum3.__doc__ == "Enum docstring"

    # options.disable_user_defined_docstrings()
    assert m.DocstringTestEnum4.__doc__ == "Members:\n\n  Member1\n\n  Member2"

    # options.disable_user_defined_docstrings()
    # options.disable_enum_members_docstring()
    # When all options are disabled, no docstring (instead of an empty one) should be generated
    assert m.DocstringTestEnum5.__doc__ is None
