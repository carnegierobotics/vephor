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
    pybind11/detail/typeid.h: Compiler-independent access to type identifiers

    Copyright (c) 2016 Wenzel Jakob <wenzel.jakob@epfl.ch>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <cstdio>
#include <cstdlib>

#if defined(__GNUG__)
#    include <cxxabi.h>
#endif

#include "common.h"

PYBIND11_NAMESPACE_BEGIN(PYBIND11_NAMESPACE)
PYBIND11_NAMESPACE_BEGIN(detail)

/// Erase all occurrences of a substring
inline void erase_all(std::string &string, const std::string &search) {
    for (size_t pos = 0;;) {
        pos = string.find(search, pos);
        if (pos == std::string::npos) {
            break;
        }
        string.erase(pos, search.length());
    }
}

PYBIND11_NOINLINE void clean_type_id(std::string &name) {
#if defined(__GNUG__)
    int status = 0;
    std::unique_ptr<char, void (*)(void *)> res{
        abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status), std::free};
    if (status == 0) {
        name = res.get();
    }
#else
    detail::erase_all(name, "class ");
    detail::erase_all(name, "struct ");
    detail::erase_all(name, "enum ");
#endif
    detail::erase_all(name, "pybind11::");
}

inline std::string clean_type_id(const char *typeid_name) {
    std::string name(typeid_name);
    detail::clean_type_id(name);
    return name;
}

PYBIND11_NAMESPACE_END(detail)

/// Return a string representation of a C++ type
template <typename T>
static std::string type_id() {
    return detail::clean_type_id(typeid(T).name());
}

PYBIND11_NAMESPACE_END(PYBIND11_NAMESPACE)
