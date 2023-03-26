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
	void cleanup()
	{
		lines->cleanup();
	}
private:
	shared_ptr<Lines> lines;
};

}