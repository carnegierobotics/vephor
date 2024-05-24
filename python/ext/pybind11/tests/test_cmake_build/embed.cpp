/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

#include <pybind11/embed.h>
namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(test_cmake_build, m) {
    m.def("add", [](int i, int j) { return i + j; });
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        throw std::runtime_error("Expected test.py file as the first argument");
    }
    auto *test_py_file = argv[1];

    py::scoped_interpreter guard{};

    auto m = py::module_::import("test_cmake_build");
    if (m.attr("add")(1, 2).cast<int>() != 3) {
        throw std::runtime_error("embed.cpp failed");
    }

    py::module_::import("sys").attr("argv") = py::make_tuple("test.py", "embed.cpp");
    py::eval_file(test_py_file, py::globals());
}
