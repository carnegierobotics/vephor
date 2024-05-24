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

#include "vephor/base/tensor.h"
#include "vephor/base/geometry.h"

namespace vephor
{

namespace mcubes
{

void calcSurfaces(const Tensor<3, float>& occupancy, float thresh, float cell_size, MeshData& data);

}

}