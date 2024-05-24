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

/* Simple test module/test class to check that the referenced internals data of external pybind11
 * modules aren't preserved over a finalize/initialize.
 */

PYBIND11_MODULE(external_module, m) {
    class A {
    public:
        explicit A(int value) : v{value} {};
        int v;
    };

    py::class_<A>(m, "A").def(py::init<int>()).def_readwrite("value", &A::v);

    m.def("internals_at",
          []() { return reinterpret_cast<uintptr_t>(&py::detail::get_internals()); });
}
