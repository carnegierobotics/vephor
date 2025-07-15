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
#include "mesh.h"

namespace vephor
{

class Skybox
{
public:
    Skybox(const shared_ptr<CubeTexture>& p_tex)
	{
		MaterialBuilder builder;
        builder.cube_tex = true;
        builder.infinite_depth = true;
        builder.materials = false;

        auto materials = builder.buildSet();

        auto data = formCube();
		mesh = make_shared<Mesh>(data, materials);
        mesh->setCull(false);
		mesh->setCubeTexture(p_tex);
	}
	void renderOGL(Window* window, const TransformSim3& world_from_body)
	{
        glDepthFunc(GL_LEQUAL);
		mesh->renderOGL(window, world_from_body * Transform3(Vec3::Zero(), Vec3(-M_PI/2,0,0)));
        glDepthFunc(GL_LESS);
	}
	void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
    {
		mesh->onAddToWindow(window, node);
	}
	void onRemoveFromWindow(Window* window)
	{
		mesh->onRemoveFromWindow(window);
	}
private:
	shared_ptr<Mesh> mesh;
};

}