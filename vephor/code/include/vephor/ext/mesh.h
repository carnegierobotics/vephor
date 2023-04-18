#pragma once

#include "window.h"

namespace vephor
{

class Mesh
{
public:
    Mesh(const MeshData& p_data)
	: data(p_data)
	{
	}
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
	void setColor(const Color& p_color){color = p_color.getRGBA();}
	void setSpecular(bool p_specular){specular = p_specular;}
	void setCull(bool p_cull){cull = p_cull;}
	json serialize(vector<vector<char>>* bufs)
	{
		return {
            {"type", "mesh"},
			{"verts", toJson(data.verts)},
			{"norms", toJson(data.norms)},
			{"uvs", toJson(data.uvs)},
			{"color_rgba", toJson(color)},
			{"tex", produceTextureData(tex_data, bufs)},
			{"specular", specular},
			{"cull", cull}
        };
	}
private:
	MeshData data;
	TextureDataRecord tex_data;
	Vec4 color = Vec4(1,1,1,1);
	float specular = 1.0f;
	bool cull = true;
};

}