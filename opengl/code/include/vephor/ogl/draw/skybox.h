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

        builder.saveShaders();

        auto material = builder.build();
        material->setCubeTexture(p_tex);

        auto data = formCube();
		mesh = make_shared<Mesh>(data, material);
        mesh->setCull(false);
	}
	void renderOGL(Window* window, const TransformSim3& world_from_body)
	{
        glDepthFunc(GL_LEQUAL);
		mesh->renderOGL(window, Transform3(Vec3::Zero(), Vec3(-M_PI/2,0,0)));
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