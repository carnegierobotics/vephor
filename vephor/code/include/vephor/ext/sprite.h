#pragma once

#include "window.h"

namespace vephor
{

class Sprite
{
public:
    Sprite(const string& p_tex, bool p_filter_nearest)
	{
		tex_data.path = p_tex;
		tex_data.filter_nearest = p_filter_nearest;
	}
	Sprite(const Image<uint8_t>& p_tex, bool p_filter_nearest)
	{
		tex_data.tex = p_tex;
		tex_data.filter_nearest = p_filter_nearest;
	}
	void setColor(const Color& p_color){color = p_color.getRGB();}
	void setFlip(bool p_x_flip, bool p_y_flip)
	{
		x_flip = p_x_flip;
		y_flip = p_y_flip;
	}
	void setNormalSpriteSheet(const string& p_tex, bool p_filter_nearest)
	{
		normal_tex_data.path = p_tex;
		normal_tex_data.filter_nearest = p_filter_nearest;
	}
	void setNormalSpriteSheet(const Image<uint8_t>& p_tex, bool p_filter_nearest)
	{
		normal_tex_data.tex = p_tex;
		normal_tex_data.filter_nearest = p_filter_nearest;
	}
	json serialize(vector<vector<char>>* bufs)
	{
		json data = {
            {"type", "sprite"},
			{"tex", produceTextureData(tex_data, bufs)},
			{"normal_tex", produceTextureData(normal_tex_data, bufs)},
			{"x_flip", x_flip},
			{"y_flip", y_flip},
			{"color_rgb", toJson(color)},
			{"diffuse", diffuse_strength},
			{"ambient", ambient_strength},
        };
		
		return data;
	}
private:
	TextureDataRecord tex_data;
	TextureDataRecord normal_tex_data;
	bool x_flip = false;
	bool y_flip = false;
	Vec3 color = Vec3(1,1,1);
	float diffuse_strength = 1.0f;
	float ambient_strength = 1.0f;
};

}