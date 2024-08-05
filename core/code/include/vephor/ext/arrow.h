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

class Arrow
{
public:
    Arrow(float p_length = 1.0f, float p_rad = 1.0f, int p_slices = 16)
	: start(0,0,-p_length/2), end(0,0,p_length/2), rad(p_rad), slices(p_slices)
	{}
	Arrow(const Vec3& p_start, const Vec3& p_end, float p_rad = 1.0f, int p_slices = 16)
	: start(p_start), end(p_end), rad(p_rad), slices(p_slices)
	{}
    void setColor(const Vec3 &p_color) { color << p_color, 1.0F; }
    void setColor(const Color &p_color) { color = p_color.getRGBA(); }
	json serialize(vector<vector<char>>*)
	{
		return {
			{"type", "arrow"},
			{"start", toJson(start)},
			{"end", toJson(end)},
			{"rad", rad},
			{"slices", slices},
			{"color_rgba", toJson(color)}
		};
	}
private:
	Vec3 start;
	Vec3 end;
	float rad;
	float slices;
	Vec4 color{1.0F, 1.0F, 1.0F, 1.0F};
};

}