/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#pragma once

#include "../window.h"
#include "material.h"

namespace vephor
{
namespace ogl
{

class GaussianSplat
{
public:
    GaussianSplat();
    void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node);
private:
    GLuint oit_fbo;
    GLuint accum_id, revealage_id;
};

} // namespace ogl
} // namespace vephor