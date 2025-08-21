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
#include "lines.h"

namespace vephor
{
namespace ogl
{

class Grid
{
public:
    Grid(float rad, 
		const Vec3& normal = Vec3(0,0,1), 
		const Vec3& right = Vec3(1,0,0), 
		float cell_size = 1.0f, 
		const Vec3& color = Vec3(1,1,1))
	{
		vector<Vec3> line_verts;
		
		Vec3 forward = right.cross(normal);
		forward /= forward.norm();

		Vec3 new_right = normal.cross(forward);
		new_right /= new_right.norm();
		
		line_verts.push_back(-rad*new_right);
		line_verts.push_back(rad*new_right);
		line_verts.push_back(-rad*forward);
		line_verts.push_back(rad*forward);
		
		float shift = cell_size;
		while (true)
		{
			if (shift > rad)
				shift = rad;
			
			line_verts.push_back(-rad*new_right+shift*forward);
			line_verts.push_back(rad*new_right+shift*forward);
			line_verts.push_back(-rad*new_right+-shift*forward);
			line_verts.push_back(rad*new_right-shift*forward);
			line_verts.push_back(-rad*forward+shift*new_right);
			line_verts.push_back(rad*forward+shift*new_right);
			line_verts.push_back(-rad*forward-shift*new_right);
			line_verts.push_back(rad*forward-shift*new_right);
			
			if (shift >= rad)
				break;
			
			shift += cell_size;
		}
		
		lines = make_shared<Lines>(toMat(line_verts), MatX(), Vec4(color[0], color[1], color[2], 1.0f));
		lines->setLineStrip(false);
	}
	void renderOGL(Window* window, const TransformSim3& world_from_body)
	{
		lines->renderOGL(window, world_from_body);
	}
	void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
    {
		lines->onAddToWindow(window, node);
	}
	void onRemoveFromWindow(Window* window)
	{
		lines->onRemoveFromWindow(window);
	}
private:
	shared_ptr<Lines> lines;
};

} // namespace ogl
} // namespace vephor