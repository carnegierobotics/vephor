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
    Arrow(float p_length = 1.0f, float p_rad = rad_default, int p_slices = slices_default)
	: start(0,0,-p_length/2), end(0,0,p_length/2), rad(p_rad), slices(p_slices)
	{}
	Arrow(const Vec3& p_start, const Vec3& p_end, float p_rad = rad_default, int p_slices = slices_default)
	: start(p_start), end(p_end), rad(p_rad), slices(p_slices)
	{}
    void setColor(const Vec3 &p_color) { color_rgba << p_color, 1.0F; }
    void setColor(const Color &p_color) { color_rgba = p_color.getRGBA(); }
	json serialize(vector<vector<char>>*)
	{
		json json_data = {
			{"type", "arrow"},
			{"start", toJson(start)},
			{"end", toJson(end)}
		};

		VEPHOR_SERIALIZE_IF_STANDARD(rad);
		VEPHOR_SERIALIZE_IF_STANDARD(slices);
		VEPHOR_SERIALIZE_IF_STANDARD(color_rgba);

		return json_data;
	}
private:
	Vec3 start;
	Vec3 end;

	inline const static float rad_default = 1.0f;
	float rad;

	inline const static int slices_default = 16;
	int slices;

	inline const static Vec4 color_rgba_default = Vec4(1,1,1,1);
	Vec4 color_rgba = color_rgba_default;
};

}