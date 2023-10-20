/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Carnegie Robotics, LLC nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CARNEGIE ROBOTICS, LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

/*
    tests/test_opaque_types.cpp -- opaque types, passing void pointers

    Copyright (c) 2016 Wenzel Jakob <wenzel.jakob@epfl.ch>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#include <pybind11/stl.h>

#include "pybind11_tests.h"

#include <vector>

// IMPORTANT: Disable internal pybind11 translation mechanisms for STL data structures
//
// This also deliberately doesn't use the below StringList type alias to test
// that MAKE_OPAQUE can handle a type containing a `,`.  (The `std::allocator`
// bit is just the default `std::vector` allocator).
PYBIND11_MAKE_OPAQUE(std::vector<std::string, std::allocator<std::string>>);

using StringList = std::vector<std::string, std::allocator<std::string>>;

TEST_SUBMODULE(opaque_types, m) {
    // test_string_list
    py::class_<StringList>(m, "StringList")
        .def(py::init<>())
        .def("pop_back", &StringList::pop_back)
        /* There are multiple versions of push_back(), etc. Select the right ones. */
        .def("push_back", (void(StringList::*)(const std::string &)) & StringList::push_back)
        .def("back", (std::string & (StringList::*) ()) & StringList::back)
        .def("__len__", [](const StringList &v) { return v.size(); })
        .def(
            "__iter__",
            [](StringList &v) { return py::make_iterator(v.begin(), v.end()); },
            py::keep_alive<0, 1>());

    class ClassWithSTLVecProperty {
    public:
        StringList stringList;
    };
    py::class_<ClassWithSTLVecProperty>(m, "ClassWithSTLVecProperty")
        .def(py::init<>())
        .def_readwrite("stringList", &ClassWithSTLVecProperty::stringList);

    m.def("print_opaque_list", [](const StringList &l) {
        std::string ret = "Opaque list: [";
        bool first = true;
        for (const auto &entry : l) {
            if (!first) {
                ret += ", ";
            }
            ret += entry;
            first = false;
        }
        return ret + "]";
    });

    // test_pointers
    m.def("return_void_ptr", []() { return (void *) 0x1234; });
    m.def("get_void_ptr_value", [](void *ptr) { return reinterpret_cast<std::intptr_t>(ptr); });
    m.def("return_null_str", []() { return (char *) nullptr; });
    m.def("get_null_str_value", [](char *ptr) { return reinterpret_cast<std::intptr_t>(ptr); });

    m.def("return_unique_ptr", []() -> std::unique_ptr<StringList> {
        auto *result = new StringList();
        result->push_back("some value");
        return std::unique_ptr<StringList>(result);
    });

    // test unions
    py::class_<IntFloat>(m, "IntFloat")
        .def(py::init<>())
        .def_readwrite("i", &IntFloat::i)
        .def_readwrite("f", &IntFloat::f);
}
