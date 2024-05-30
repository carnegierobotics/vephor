/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

/*
    tests/test_class.cpp -- test py::class_ definitions and basic functionality

    Copyright (c) 2019 Roland Dreier <roland.dreier@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#include "pybind11_tests.h"

TEST_SUBMODULE(union_, m) {
    union TestUnion {
        int value_int;
        unsigned value_uint;
    };

    py::class_<TestUnion>(m, "TestUnion")
        .def(py::init<>())
        .def_readonly("as_int", &TestUnion::value_int)
        .def_readwrite("as_uint", &TestUnion::value_uint);
}
