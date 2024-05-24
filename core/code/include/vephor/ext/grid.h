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

#include "window.h"

namespace vephor
{

class Grid
{
public:
    Grid(float p_rad, const Vec3& p_normal = Vec3(0,0,1), const Vec3& p_right = Vec3(1,0,0), float p_cell_size = 1.0f)
	: rad(p_rad), cell_size(p_cell_size), normal(p_normal), right(p_right)
	{}
	void setColor(const Vec3& p_color){color = p_color;}
	json serialize(vector<vector<char>>*)
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