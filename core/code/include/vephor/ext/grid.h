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
    Grid(float p_rad = rad_default, 
		const Vec3& p_normal = normal_default, 
		const Vec3& p_right = right_default, 
		float p_cell_size = cell_size_default)
	: rad(p_rad), cell_size(p_cell_size), normal(p_normal), right(p_right)
	{}
	void setColor(const Vec3& p_color){color_rgb = p_color;}
	json serialize(vector<vector<char>>*)
	{
		json json_data = {
            {"type", "grid"}
		};

		VEPHOR_SERIALIZE_IF_STANDARD(rad);
		VEPHOR_SERIALIZE_IF_STANDARD(cell_size);
		VEPHOR_SERIALIZE_IF_STANDARD(normal);
		VEPHOR_SERIALIZE_IF_STANDARD(right);
		VEPHOR_SERIALIZE_IF_STANDARD(color_rgb);

		return json_data;
	}
private:
	inline const static float rad_default = 10.0f;
	float rad;

	inline const static float cell_size_default = 1.0f;
	float cell_size;

	inline const static Vec3 normal_default = Vec3(0,0,1);
	Vec3 normal;

	inline const static Vec3 right_default = Vec3(1,0,0);
	Vec3 right;

	inline const static Vec3 color_rgb_default = Vec3(1,1,1);
	Vec3 color_rgb = Vec3(1,1,1);
};

}