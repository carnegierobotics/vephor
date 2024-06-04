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
#include "cylinder.h"

namespace vephor
{

class Axes
{
public:
    Axes(float p_size = 1.0f)
	: size(p_size)
	{}
	void setColors(
		const Color& p_color_x,
		const Color& p_color_y,
		const Color& p_color_z
	)
	{
		colors.resize(3);
		colors[0] = p_color_x.getRGB();
		colors[1] = p_color_y.getRGB();
		colors[2] = p_color_z.getRGB();
	}
	void setColorsCMY()
	{
		setColors(
			Vec3(0,1,1),
			Vec3(1,0,1),
			Vec3(1,1,0)
		);
	}
	json serialize(vector<vector<char>>*)
	{
		json data = {
			{"type", "axes"},
			{"size", size}
		};

		if (!colors.empty())
		{
			data["x_rgb"] = toJson(colors[0]);
			data["y_rgb"] = toJson(colors[1]);
			data["z_rgb"] = toJson(colors[2]);
		}

		return data;
	}
private:
	float size;
	vector<Vec3> colors;
};

}