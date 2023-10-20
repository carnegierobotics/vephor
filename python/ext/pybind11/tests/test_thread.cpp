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
    tests/test_thread.cpp -- call pybind11 bound methods in threads

    Copyright (c) 2021 Laramie Leavitt (Google LLC) <lar@google.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#include <pybind11/cast.h>
#include <pybind11/pybind11.h>

#include "pybind11_tests.h"

#include <chrono>
#include <thread>

namespace py = pybind11;

namespace {

struct IntStruct {
    explicit IntStruct(int v) : value(v){};
    ~IntStruct() { value = -value; }
    IntStruct(const IntStruct &) = default;
    IntStruct &operator=(const IntStruct &) = default;

    int value;
};

} // namespace

TEST_SUBMODULE(thread, m) {

    py::class_<IntStruct>(m, "IntStruct").def(py::init([](const int i) { return IntStruct(i); }));

    // implicitly_convertible uses loader_life_support when an implicit
    // conversion is required in order to lifetime extend the reference.
    //
    // This test should be run with ASAN for better effectiveness.
    py::implicitly_convertible<int, IntStruct>();

    m.def("test", [](int expected, const IntStruct &in) {
        {
            py::gil_scoped_release release;
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        if (in.value != expected) {
            throw std::runtime_error("Value changed!!");
        }
    });

    m.def(
        "test_no_gil",
        [](int expected, const IntStruct &in) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            if (in.value != expected) {
                throw std::runtime_error("Value changed!!");
            }
        },
        py::call_guard<py::gil_scoped_release>());

    // NOTE: std::string_view also uses loader_life_support to ensure that
    // the string contents remain alive, but that's a C++ 17 feature.
}
