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
    tests/eigen_tensor.cpp -- automatic conversion of Eigen Tensor

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#ifndef EIGEN_AVOID_STL_ARRAY
#    define EIGEN_AVOID_STL_ARRAY
#endif

#include "test_eigen_tensor.inl"

PYBIND11_MODULE(eigen_tensor_avoid_stl_array, m) { eigen_tensor_test::test_module(m); }
