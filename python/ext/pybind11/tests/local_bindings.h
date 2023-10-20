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

#pragma once
#include "pybind11_tests.h"

#include <utility>

/// Simple class used to test py::local:
template <int>
class LocalBase {
public:
    explicit LocalBase(int i) : i(i) {}
    int i = -1;
};

/// Registered with py::module_local in both main and secondary modules:
using LocalType = LocalBase<0>;
/// Registered without py::module_local in both modules:
using NonLocalType = LocalBase<1>;
/// A second non-local type (for stl_bind tests):
using NonLocal2 = LocalBase<2>;
/// Tests within-module, different-compilation-unit local definition conflict:
using LocalExternal = LocalBase<3>;
/// Mixed: registered local first, then global
using MixedLocalGlobal = LocalBase<4>;
/// Mixed: global first, then local
using MixedGlobalLocal = LocalBase<5>;

/// Registered with py::module_local only in the secondary module:
using ExternalType1 = LocalBase<6>;
using ExternalType2 = LocalBase<7>;

using LocalVec = std::vector<LocalType>;
using LocalVec2 = std::vector<NonLocal2>;
using LocalMap = std::unordered_map<std::string, LocalType>;
using NonLocalVec = std::vector<NonLocalType>;
using NonLocalVec2 = std::vector<NonLocal2>;
using NonLocalMap = std::unordered_map<std::string, NonLocalType>;
using NonLocalMap2 = std::unordered_map<std::string, uint8_t>;

// Exception that will be caught via the module local translator.
class LocalException : public std::exception {
public:
    explicit LocalException(const char *m) : message{m} {}
    const char *what() const noexcept override { return message.c_str(); }

private:
    std::string message = "";
};

// Exception that will be registered with register_local_exception_translator
class LocalSimpleException : public std::exception {
public:
    explicit LocalSimpleException(const char *m) : message{m} {}
    const char *what() const noexcept override { return message.c_str(); }

private:
    std::string message = "";
};

PYBIND11_MAKE_OPAQUE(LocalVec);
PYBIND11_MAKE_OPAQUE(LocalVec2);
PYBIND11_MAKE_OPAQUE(LocalMap);
PYBIND11_MAKE_OPAQUE(NonLocalVec);
// PYBIND11_MAKE_OPAQUE(NonLocalVec2); // same type as LocalVec2
PYBIND11_MAKE_OPAQUE(NonLocalMap);
PYBIND11_MAKE_OPAQUE(NonLocalMap2);

// Simple bindings (used with the above):
template <typename T, int Adjust = 0, typename... Args>
py::class_<T> bind_local(Args &&...args) {
    return py::class_<T>(std::forward<Args>(args)...).def(py::init<int>()).def("get", [](T &i) {
        return i.i + Adjust;
    });
};

// Simulate a foreign library base class (to match the example in the docs):
namespace pets {
class Pet {
public:
    explicit Pet(std::string name) : name_(std::move(name)) {}
    std::string name_;
    const std::string &name() const { return name_; }
};
} // namespace pets

struct MixGL {
    int i;
    explicit MixGL(int i) : i{i} {}
};
struct MixGL2 {
    int i;
    explicit MixGL2(int i) : i{i} {}
};
