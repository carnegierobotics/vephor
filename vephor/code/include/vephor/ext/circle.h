#pragma once

#include "window.h"
#include "mesh.h"

namespace vephor
{

class Circle
{
public:
    Circle(float p_rad = 1.0f, float p_thickness = 1.0f, float p_slices = 16)
	: rad(p_rad), thickness(p_thickness), slices(p_slices)
	{}
	void setColor(const Vec3& p_color){color = p_color;}
	json serialize(vector<vector<char>>* bufs)
	{	
		return {
            {"type", "circle"},
			{"color_rgb", toJson(color)},
			{"rad", rad},
			{"thickness", thickness},
			{"slices", slices}
        };
	}
private:
	float rad;
	float thickness;
	int slices;
	Vec3 color = Vec3(1,1,1);
};

}