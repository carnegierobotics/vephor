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
	void setColor(const Color& p_color){color = p_color.getRGBA();}
	void setTexture(const string& p_tex, bool p_filter_nearest)
	{
		tex_data.path = p_tex;
		tex_data.filter_nearest = p_filter_nearest;
	}
	void setTexture(const Image<uint8_t>& p_tex, bool p_filter_nearest)
	{
		tex_data.tex = p_tex;
		tex_data.filter_nearest = p_filter_nearest;
	}
	json serialize(vector<vector<char>>* bufs)
	{	
		return {
            {"type", "plane"},
			{"tex", produceTextureData(tex_data, bufs)},
			{"rads", toJson(rads)},
			{"color_rgba", toJson(color)}
        };
	}
private:
	Vec2 rads;
	TextureDataRecord tex_data;
	Vec4 color = Vec4(1,1,1,1);
};

}