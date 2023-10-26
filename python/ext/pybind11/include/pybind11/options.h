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
    pybind11/options.h: global settings that are configurable at runtime.

    Copyright (c) 2016 Wenzel Jakob <wenzel.jakob@epfl.ch>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include "detail/common.h"

PYBIND11_NAMESPACE_BEGIN(PYBIND11_NAMESPACE)

class options {
public:
    // Default RAII constructor, which leaves settings as they currently are.
    options() : previous_state(global_state()) {}

    // Class is non-copyable.
    options(const options &) = delete;
    options &operator=(const options &) = delete;

    // Destructor, which restores settings that were in effect before.
    ~options() { global_state() = previous_state; }

    // Setter methods (affect the global state):

    options &disable_user_defined_docstrings() & {
        global_state().show_user_defined_docstrings = false;
        return *this;
    }

    options &enable_user_defined_docstrings() & {
        global_state().show_user_defined_docstrings = true;
        return *this;
    }

    options &disable_function_signatures() & {
        global_state().show_function_signatures = false;
        return *this;
    }

    options &enable_function_signatures() & {
        global_state().show_function_signatures = true;
        return *this;
    }

    options &disable_enum_members_docstring() & {
        global_state().show_enum_members_docstring = false;
        return *this;
    }

    options &enable_enum_members_docstring() & {
        global_state().show_enum_members_docstring = true;
        return *this;
    }

    // Getter methods (return the global state):

    static bool show_user_defined_docstrings() {
        return global_state().show_user_defined_docstrings;
    }

    static bool show_function_signatures() { return global_state().show_function_signatures; }

    static bool show_enum_members_docstring() {
        return global_state().show_enum_members_docstring;
    }

    // This type is not meant to be allocated on the heap.
    void *operator new(size_t) = delete;

private:
    struct state {
        bool show_user_defined_docstrings = true; //< Include user-supplied texts in docstrings.
        bool show_function_signatures = true;     //< Include auto-generated function signatures
                                                  //  in docstrings.
        bool show_enum_members_docstring = true;  //< Include auto-generated member list in enum
                                                  //  docstrings.
    };

    static state &global_state() {
        static state instance;
        return instance;
    }

    state previous_state;
};

PYBIND11_NAMESPACE_END(PYBIND11_NAMESPACE)
