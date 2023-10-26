/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Carnegie Robotics, LLC nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CARNEGIE ROBOTICS, LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

#pragma once

#include "window.h"
#include "lines.h"

namespace vephor
{

class Grid
{
public:
    Grid(float rad, const Vec3& normal = Vec3(0,0,1), const Vec3& right = Vec3(1,0,0), float cell_size = 1.0f, const Vec3& color = Vec3(1,1,1))
	{
		vector<Vec3> line_verts;
		
		Vec3 forward = right.cross(normal);
		forward /= forward.norm();
		
		line_verts.push_back(-rad*right);
		line_verts.push_back(rad*right);
		line_verts.push_back(-rad*forward);
		line_verts.push_back(rad*forward);
		
		float shift = cell_size;
		while (true)
		{
			if (shift > rad)
				shift = rad;
			
			line_verts.push_back(-rad*right+shift*forward);
			line_verts.push_back(rad*right+shift*forward);
			line_verts.push_back(-rad*right+-shift*forward);
			line_verts.push_back(rad*right-shift*forward);
			line_verts.push_back(-rad*forward+shift*right);
			line_verts.push_back(rad*forward+shift*right);
			line_verts.push_back(-rad*forward-shift*right);
			line_verts.push_back(rad*forward-shift*right);
			
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

}