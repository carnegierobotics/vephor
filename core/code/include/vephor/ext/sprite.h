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

namespace vephor
{

class Sprite
{
public:
    Sprite(const string& p_tex, bool p_filter_nearest = false)
	{
		tex_data.path = p_tex;
		tex_data.filter_nearest = p_filter_nearest;
	}
	Sprite(const Image<uint8_t>& p_tex, bool p_filter_nearest = false)
	{
		if (p_tex.getChannels() == 1)
		{
			tex_data.tex = p_tex.changeChannels(3);
		}
		else
		{
			tex_data.tex = p_tex;
		}

		tex_data.filter_nearest = p_filter_nearest;
	}
	void setColor(const Color& p_color){color_rgb = p_color.getRGB();}
	void setFlip(bool p_x_flip, bool p_y_flip)
	{
		x_flip = p_x_flip;
		y_flip = p_y_flip;
	}
	void setNormalSpriteSheet(const string& p_tex, bool p_filter_nearest = false)
	{
		normal_tex_data.path = p_tex;
		normal_tex_data.filter_nearest = p_filter_nearest;
	}
	void setNormalSpriteSheet(const Image<uint8_t>& p_tex, bool p_filter_nearest = false)
	{
		normal_tex_data.tex = p_tex;
		normal_tex_data.filter_nearest = p_filter_nearest;
	}
	json serialize(vector<vector<char>>* bufs)
	{
		json json_data = {
            {"type", "sprite"}
        };

		json json_tex_data = produceTextureData(tex_data, bufs);
		if (json_tex_data["type"] != "none")
			json_data["tex"] = json_tex_data;

		json json_normal_tex_data = produceTextureData(normal_tex_data, bufs);
		if (json_normal_tex_data["type"] != "none")
			json_data["normal_tex"] = json_normal_tex_data;

		VEPHOR_SERIALIZE_IF_STANDARD(x_flip);
		VEPHOR_SERIALIZE_IF_STANDARD(y_flip);
		VEPHOR_SERIALIZE_IF_STANDARD(color_rgb);
		VEPHOR_SERIALIZE_IF_STANDARD(diffuse);
		VEPHOR_SERIALIZE_IF_STANDARD(ambient);
		
		return json_data;
	}
private:
	TextureDataRecord tex_data;
	TextureDataRecord normal_tex_data;

	inline const static bool x_flip_default = false;
	bool x_flip = x_flip_default;

	inline const static bool y_flip_default = false;
	bool y_flip = y_flip_default;

	inline const static Vec3 color_rgb_default = Vec3(1,1,1);
	Vec3 color_rgb = color_rgb_default;

	inline const static float diffuse_default = 1.0f;
	float diffuse = diffuse_default;

	inline const static float ambient_default = 1.0f;
	float ambient = ambient_default;
};

}