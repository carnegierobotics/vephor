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

// Copyright (c) 2021 The Pybind Development Team.
// All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#include "pybind11_tests.h"

// IUT = Implementation Under Test
#define CONST_NAME_TESTS(TEST_FUNC, IUT)                                                          \
    std::string TEST_FUNC(int selector) {                                                         \
        switch (selector) {                                                                       \
            case 0:                                                                               \
                return IUT("").text;                                                              \
            case 1:                                                                               \
                return IUT("A").text;                                                             \
            case 2:                                                                               \
                return IUT("Bd").text;                                                            \
            case 3:                                                                               \
                return IUT("Cef").text;                                                           \
            case 4:                                                                               \
                return IUT<int>().text; /*NOLINT(bugprone-macro-parentheses)*/                    \
            case 5:                                                                               \
                return IUT<std::string>().text; /*NOLINT(bugprone-macro-parentheses)*/            \
            case 6:                                                                               \
                return IUT<true>("T1", "T2").text; /*NOLINT(bugprone-macro-parentheses)*/         \
            case 7:                                                                               \
                return IUT<false>("U1", "U2").text; /*NOLINT(bugprone-macro-parentheses)*/        \
            case 8:                                                                               \
                /*NOLINTNEXTLINE(bugprone-macro-parentheses)*/                                    \
                return IUT<true>(IUT("D1"), IUT("D2")).text;                                      \
            case 9:                                                                               \
                /*NOLINTNEXTLINE(bugprone-macro-parentheses)*/                                    \
                return IUT<false>(IUT("E1"), IUT("E2")).text;                                     \
            case 10:                                                                              \
                return IUT("KeepAtEnd").text;                                                     \
            default:                                                                              \
                break;                                                                            \
        }                                                                                         \
        throw std::runtime_error("Invalid selector value.");                                      \
    }

CONST_NAME_TESTS(const_name_tests, py::detail::const_name)

#ifdef PYBIND11_DETAIL_UNDERSCORE_BACKWARD_COMPATIBILITY
CONST_NAME_TESTS(underscore_tests, py::detail::_)
#endif

TEST_SUBMODULE(const_name, m) {
    m.def("const_name_tests", const_name_tests);

#if defined(PYBIND11_DETAIL_UNDERSCORE_BACKWARD_COMPATIBILITY)
    m.def("underscore_tests", underscore_tests);
#else
    m.attr("underscore_tests") = "PYBIND11_DETAIL_UNDERSCORE_BACKWARD_COMPATIBILITY not defined.";
#endif
}
