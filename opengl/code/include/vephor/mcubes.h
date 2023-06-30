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