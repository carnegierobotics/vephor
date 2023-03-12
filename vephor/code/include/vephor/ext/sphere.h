#pragma once

#include "window.h"
#include "mesh.h"

namespace vephor
{

class Sphere
{
public:
    Sphere(float p_rad = 1.0f, int p_slices = 16, int p_stacks = 16)
	: rad(p_rad), slices(p_slices), stacks(p_stacks)
	{}
	void setColor(const Vec3& p_color){color = p_color;}
	json serialize(vector<vector<char>>* bufs)
	{	
		return {
            {"type", "sphere"},
			{"color_rgb", toJson(color)},
			{"rad", rad},
			{"slices", slices},
			{"stacks", stacks}
        };
	}
private:
	float rad;
	int slices;
	int stacks;
	Vec3 color = Vec3(1,1,1);
};

}