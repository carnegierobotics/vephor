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

class Particle
{
public:
	Particle(const MatXRef& p_verts, const MatXRef& p_colors = MatX())
	: verts(p_verts.transpose()), colors(p_colors.transpose())
	{
	}
	Particle(const vector<Vec3>& p_verts, const vector<Vec4>& p_colors = vector<Vec4>())
	{
		verts = MatXMap(reinterpret_cast<const float*>(p_verts.data()), 3, p_verts.size());
		
		if (!p_colors.empty())
			colors = MatXMap(reinterpret_cast<const float*>(p_colors.data()), 4, p_colors.size());
	}
	void setSize(float p_size){size = p_size;}
	void setColor(const Color& p_color){default_color_rgba = p_color.getRGBA();}
	void setTexture(const string& p_tex, bool p_filter_nearest = nearest_default)
	{
		string temp_dir = getTempDir();
		
		if (!fs::exists(temp_dir+"/scene_assets"))
			fs::create_directory(temp_dir+"/scene_assets");

		if (!fs::exists(p_tex))
		{
			v4print "Error: Can't find particle texture", p_tex, "- skipping";
			return;
		}
		
		string final_path = temp_dir+"/scene_assets/"+fs::path(p_tex).filename().string();
		if (!fs::exists(final_path))
			fs::copy(p_tex, final_path);
		
		tex = "scene_assets/"+fs::path(p_tex).filename().string();
		nearest = p_filter_nearest;
	}
	void setScreenSpaceMode(bool p_ss_mode)
	{
		ss_mode = p_ss_mode;
	}
	json serialize(vector<vector<char>>* bufs)
	{	
		json json_data = {
			{"type", "particle"}
		};

		if (!tex.empty())
		{
			json_data["tex"] = tex;
			VEPHOR_SERIALIZE_IF_STANDARD(nearest);
		}
		VEPHOR_SERIALIZE_IF_STANDARD(default_color_rgba);
		VEPHOR_SERIALIZE_IF_STANDARD(size);
		VEPHOR_SERIALIZE_IF_STANDARD(ss_mode);
		
		if (bufs)
		{
			json_data["verts"] = produceVertDataRaw(verts, bufs);
		}
		else
		{
			json_data["verts"] = produceVertDataBase64(verts);
		}

		if (colors.rows() > 0)
		{
			//{"colors", toJson(colors)},
			if (bufs)
			{
				json_data["colors"] = produceVertDataRaw(colors, bufs);
			}
			else
			{
				json_data["colors"] = produceVertDataBase64(colors);
			}
		}

		return json_data;
	}
private:
	MatX verts;
	MatX colors;
	string tex;

	inline const static bool nearest_default = false;
	bool nearest = nearest_default;

	inline const static Vec4 default_color_rgba_default = Vec4(1,1,1,1);
	Vec4 default_color_rgba = default_color_rgba_default;

	inline const static float size_default = 0.03f;
	float size = size_default;

	inline const static bool ss_mode_default = false;
	bool ss_mode = ss_mode_default;
};

}