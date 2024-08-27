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

namespace vephor
{

class Mesh
{
public:
    Mesh(const MeshData& p_data)
	: data(p_data)
	{
		if (data.verts.rows() == 0)
			throw std::runtime_error("Empty mesh created.");
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
	void setDiffuseStrength(float strength){diffuse_strength = strength;}
	void setAmbientStrength(float strength){ambient_strength = strength;}
	void setEmissiveStrength(float strength){emissive_strength = strength;}
	json serialize(vector<vector<char>>* bufs)
	{
		return {
            {"type", "mesh"},
			{"verts", toJson(data.verts)},
			{"norms", toJson(data.norms)},
			{"uvs", toJson(data.uvs)},
			{"color_rgba", toJson(color)},
			{"diffuse", diffuse_strength},
			{"ambient", ambient_strength},
			{"emissive", emissive_strength},
			{"tex", produceTextureData(tex_data, bufs)},
			{"specular", specular},
			{"cull", cull}
        };
	}
private:
	MeshData data;
	TextureDataRecord tex_data;
	Vec4 color = Vec4(1,1,1,1);
	float diffuse_strength = 1.0f;
	float ambient_strength = 1.0f;
	float emissive_strength = 0.0f;
	float specular = 1.0f;
	bool cull = true;
};

}