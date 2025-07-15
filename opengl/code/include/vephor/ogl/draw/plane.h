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
namespace ogl
{

class Plane
{
public:
    Plane(const Vec2& p_size)
	{
		auto data = formPlane(p_size);

		MaterialBuilder builder;
		builder.dir_light = true;
		builder.point_lights = true;
		builder.tex = true;
		mesh = make_shared<Mesh>(data, builder.buildSet());
	}
	void setColor(const Color& p_color)
	{
		mesh->setDiffuse(p_color.getRGB());
		mesh->setAmbient(p_color.getRGB());
		mesh->setOpacity(p_color.getAlpha());
	}
	void setTexture(const shared_ptr<Texture>& p_tex){mesh->setTexture(p_tex);}
	void renderOGL(Window* window, const TransformSim3& world_from_body)
	{
		mesh->renderOGL(window, world_from_body);
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

} // namespace ogl
} // namespace vephor