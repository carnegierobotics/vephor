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
    tests/test_eval.cpp -- Usage of eval() and eval_file()

    Copyright (c) 2016 Klemens D. Morgenstern

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#include <pybind11/eval.h>

#include "pybind11_tests.h"

#include <utility>

TEST_SUBMODULE(eval_, m) {
    // test_evals

    auto global = py::dict(py::module_::import("__main__").attr("__dict__"));

    m.def("test_eval_statements", [global]() {
        auto local = py::dict();
        local["call_test"] = py::cpp_function([&]() -> int { return 42; });

        // Regular string literal
        py::exec("message = 'Hello World!'\n"
                 "x = call_test()",
                 global,
                 local);

        // Multi-line raw string literal
        py::exec(R"(
            if x == 42:
                print(message)
            else:
                raise RuntimeError
            )",
                 global,
                 local);
        auto x = local["x"].cast<int>();

        return x == 42;
    });

    m.def("test_eval", [global]() {
        auto local = py::dict();
        local["x"] = py::int_(42);
        auto x = py::eval("x", global, local);
        return x.cast<int>() == 42;
    });

    m.def("test_eval_single_statement", []() {
        auto local = py::dict();
        local["call_test"] = py::cpp_function([&]() -> int { return 42; });

        auto result = py::eval<py::eval_single_statement>("x = call_test()", py::dict(), local);
        auto x = local["x"].cast<int>();
        return result.is_none() && x == 42;
    });

    m.def("test_eval_file", [global](py::str filename) {
        auto local = py::dict();
        local["y"] = py::int_(43);

        int val_out = 0;
        local["call_test2"] = py::cpp_function([&](int value) { val_out = value; });

        auto result = py::eval_file(std::move(filename), global, local);
        return val_out == 43 && result.is_none();
    });

    m.def("test_eval_failure", []() {
        try {
            py::eval("nonsense code ...");
        } catch (py::error_already_set &) {
            return true;
        }
        return false;
    });

    m.def("test_eval_file_failure", []() {
        try {
            py::eval_file("non-existing file");
        } catch (std::exception &) {
            return true;
        }
        return false;
    });

    // test_eval_empty_globals
    m.def("eval_empty_globals", [](py::object global) {
        if (global.is_none()) {
            global = py::dict();
        }
        auto int_class = py::eval("isinstance(42, int)", global);
        return global;
    });

    // test_eval_closure
    m.def("test_eval_closure", []() {
        py::dict global;
        global["closure_value"] = 42;
        py::dict local;
        local["closure_value"] = 0;
        py::exec(R"(
            local_value = closure_value

            def func_global():
                return closure_value

            def func_local():
                return local_value
            )",
                 global,
                 local);
        return std::make_pair(global, local);
    });
}
