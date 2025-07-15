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

#include "window.h"
#include "mesh.h"

namespace vephor
{

class Circle
{
public:
    Circle(float p_rad = rad_default, float p_thickness = thickness_default, float p_slices = slices_default)
	: rad(p_rad), thickness(p_thickness), slices(p_slices)
	{}
	void setColor(const Vec3& p_color){color_rgb = p_color;}
	json serialize(vector<vector<char>>*)
	{	
		json json_data = {
            {"type", "circle"}
        };

		VEPHOR_SERIALIZE_IF_STANDARD(rad);
		VEPHOR_SERIALIZE_IF_STANDARD(thickness);
		VEPHOR_SERIALIZE_IF_STANDARD(slices);
		VEPHOR_SERIALIZE_IF_STANDARD(color_rgb);

		return json_data;
	}
private:
	inline const static float rad_default = 1.0f;
	float rad;

	inline const static float thickness_default = 1.0f;
	float thickness;

	inline const static int slices_default = 16;
	int slices;

	inline const static Vec3 color_rgb_default = Vec3(1,1,1);
	Vec3 color_rgb = color_rgb_default;
};

}