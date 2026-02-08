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

class Cylinder
{
public:
    Cylinder(float p_rad = rad_default, float p_height = height_default, int p_slices = slices_default)
	: rad(p_rad), height(p_height), slices(p_slices)
	{}
	void setColor(const Color& p_color){color_rgb = p_color.getRGB();}
	json serialize(vector<vector<char>>*)
	{	
		json json_data = {
            {"type", "cylinder"}
        };

		VEPHOR_SERIALIZE_IF_STANDARD(rad);
		VEPHOR_SERIALIZE_IF_STANDARD(height);
		VEPHOR_SERIALIZE_IF_STANDARD(slices);
		VEPHOR_SERIALIZE_IF_STANDARD(color_rgb);

		return json_data;
	}
private:
	inline const static float rad_default = 1.0f;
	float rad;

	inline const static float height_default = 1.0f;
	float height;

	inline const static int slices_default = 16;
	int slices;

	inline const static Vec3 color_rgb_default = Vec3(1,1,1);
	Vec3 color_rgb = color_rgb_default;
};

}