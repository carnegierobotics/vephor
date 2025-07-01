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
#include "mesh.h"

namespace vephor
{

class Cone
{
public:
    Cone(float p_rad = rad_default, float p_height = height_default, int p_slices = slices_default)
	: rad(p_rad), height(p_height), slices(p_slices)
	{}
	void setColor(const Color& p_color){color_rgb = p_color.getRGB();}
	json serialize(vector<vector<char>>*)
	{	
		json json_data = {
            {"type", "cone"}
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