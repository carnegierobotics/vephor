#pragma once

#include "window.h"

namespace vephor
{

class Grid
{
public:
    Grid(float p_rad, const Vec3& p_normal = Vec3(0,0,1), const Vec3& p_right = Vec3(1,0,0), float p_cell_size = 1.0f)
	: rad(p_rad), normal(p_normal), right(p_right), cell_size(p_cell_size)
	{}
	void setColor(const Vec3& p_color){color = p_color;}
	json serialize(vector<vector<char>>* bufs)
	{
		return {
            {"type", "grid"},
			{"rad", rad},
			{"cell_size", cell_size},
			{"normal", toJson(normal)},
			{"right", toJson(right)},
			{"color_rgb", toJson(color)}
		};
	}
private:
	float rad;
	float cell_size;
	Vec3 normal;
	Vec3 right;
	Vec3 color = Vec3(1,1,1);
};

}