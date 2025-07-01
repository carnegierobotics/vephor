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
	void setColor(const Color& p_color){color_rgba = p_color.getRGBA();}
	void setSpecular(bool p_specular){specular = p_specular;}
	void setCull(bool p_cull){cull = p_cull;}
	void setDiffuseStrength(float strength){diffuse = strength;}
	void setAmbientStrength(float strength){ambient = strength;}
	void setEmissiveStrength(float strength){emissive = strength;}
	json serialize(vector<vector<char>>* bufs)
	{
		json json_data = {
            {"type", "mesh"},
			{"verts", toJson(data.verts)},
			{"norms", toJson(data.norms)},
			{"uvs", toJson(data.uvs)}
        };

		json json_tex_data = produceTextureData(tex_data, bufs);
		if (json_tex_data["type"] != "none")
			json_data["tex"] = json_tex_data;

		VEPHOR_SERIALIZE_IF_STANDARD(color_rgba);
		VEPHOR_SERIALIZE_IF_STANDARD(diffuse);
		VEPHOR_SERIALIZE_IF_STANDARD(ambient);
		VEPHOR_SERIALIZE_IF_STANDARD(emissive);
		VEPHOR_SERIALIZE_IF_STANDARD(specular);
		VEPHOR_SERIALIZE_IF_STANDARD(cull);

		return json_data;
	}
private:
	MeshData data;
	TextureDataRecord tex_data;

	inline const static Vec4 color_rgba_default = Vec4(1,1,1,1);
	Vec4 color_rgba = color_rgba_default;

	inline const static float diffuse_default = 1.0f;
	float diffuse = diffuse_default;

	inline const static float ambient_default = 1.0f;
	float ambient = ambient_default;

	inline const static float emissive_default = 0.0f;
	float emissive = emissive_default;

	inline const static float specular_default = 1.0f;
	float specular = specular_default;

	inline const static bool cull_default = true;
	bool cull = cull_default;
};

}