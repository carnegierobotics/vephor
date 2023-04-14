#pragma once

#include "window.h"
#include "mesh.h"

namespace vephor
{

class Cylinder
{
public:
    Cylinder(float p_rad = 1.0f, float p_height = 1.0f, int p_slices = 16)
	: rad(p_rad), height(p_height), slices(p_slices)
	{}
	void setColor(const Vec3& p_color){color = p_color;}
	json serialize(vector<vector<char>>*)
	{	
		return {
            {"type", "cylinder"},
			{"color_rgb", toJson(color)},
			{"rad", rad},
			{"height", height},
			{"slices", slices}
        };
	}
private:
	float rad;
	float height;
	int slices;
	Vec3 color = Vec3(1,1,1);
};

}