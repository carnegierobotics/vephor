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
	
class Plane
{
public:
    Plane(const Vec2& p_rads)
	: rads(p_rads)
	{}
	void setColor(const Color& p_color){color_rgba = p_color.getRGBA();}
	void setTexture(const string& p_tex, bool p_filter_nearest = false)
	{
		tex_data.path = p_tex;
		tex_data.filter_nearest = p_filter_nearest;
	}
	void setTexture(const Image<uint8_t>& p_tex, bool p_filter_nearest = false)
	{
		tex_data.tex = p_tex;
		tex_data.filter_nearest = p_filter_nearest;
	}
	json serialize(vector<vector<char>>* bufs)
	{	
		json json_data {
            {"type", "plane"},
			{"rads", toJson(rads)}
        };

		json json_tex_data = produceTextureData(tex_data, bufs);
		if (json_tex_data["type"] != "none")
			json_data["tex"] = json_tex_data;

		VEPHOR_SERIALIZE_IF_STANDARD(color_rgba);

		return json_data;
	}
private:
	Vec2 rads;
	TextureDataRecord tex_data;

	inline const static Vec4 color_rgba_default = Vec4(1,1,1,1);
	Vec4 color_rgba = Vec4(1,1,1,1);
};

}