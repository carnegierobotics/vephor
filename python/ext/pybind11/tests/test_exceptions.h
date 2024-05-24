/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

#pragma once
#include "pybind11_tests.h"

#include <stdexcept>

// shared exceptions for cross_module_tests

class PYBIND11_EXPORT_EXCEPTION shared_exception : public pybind11::builtin_exception {
public:
    using builtin_exception::builtin_exception;
    explicit shared_exception() : shared_exception("") {}
    void set_error() const override { PyErr_SetString(PyExc_RuntimeError, what()); }
};
