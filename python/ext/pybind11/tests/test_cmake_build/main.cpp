/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

#include <pybind11/pybind11.h>
namespace py = pybind11;

PYBIND11_MODULE(test_cmake_build, m) {
    m.def("add", [](int i, int j) { return i + j; });
}
