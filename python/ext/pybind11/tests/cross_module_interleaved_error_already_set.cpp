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
    Copyright (c) 2022 Google LLC

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#include <pybind11/pybind11.h>

// This file mimics a DSO that makes pybind11 calls but does not define a PYBIND11_MODULE,
// so that the first call of cross_module_error_already_set() triggers the first call of
// pybind11::detail::get_internals().

namespace {

namespace py = pybind11;

void interleaved_error_already_set() {
    PyErr_SetString(PyExc_RuntimeError, "1st error.");
    try {
        throw py::error_already_set();
    } catch (const py::error_already_set &) {
        // The 2nd error could be conditional in a real application.
        PyErr_SetString(PyExc_RuntimeError, "2nd error.");
    } // Here the 1st error is destroyed before the 2nd error is fetched.
    // The error_already_set dtor triggers a pybind11::detail::get_internals()
    // call via pybind11::gil_scoped_acquire.
    if (PyErr_Occurred()) {
        throw py::error_already_set();
    }
}

constexpr char kModuleName[] = "cross_module_interleaved_error_already_set";

struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT, kModuleName, nullptr, 0, nullptr, nullptr, nullptr, nullptr, nullptr};

} // namespace

extern "C" PYBIND11_EXPORT PyObject *PyInit_cross_module_interleaved_error_already_set() {
    PyObject *m = PyModule_Create(&moduledef);
    if (m != nullptr) {
        static_assert(sizeof(&interleaved_error_already_set) == sizeof(void *),
                      "Function pointer must have the same size as void *");
        PyModule_AddObject(
            m,
            "funcaddr",
            PyLong_FromVoidPtr(reinterpret_cast<void *>(&interleaved_error_already_set)));
    }
    return m;
}
