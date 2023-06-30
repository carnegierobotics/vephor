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
	void setColor(const Color& p_color){default_color = p_color.getRGBA();}
	void setTexture(const string& p_tex, bool p_filter_nearest = false)
	{
		string temp_dir = getTempDir();
		
		if (!fs::exists(temp_dir+"/scene_assets"))
			fs::create_directory(temp_dir+"/scene_assets");
		
		string final_path = temp_dir+"/scene_assets/"+fs::path(p_tex).filename().string();
		if (!fs::exists(final_path))
			fs::copy(p_tex, final_path);
		
		tex = "scene_assets/"+fs::path(p_tex).filename().string();
		filter_nearest = p_filter_nearest;
	}
	void setScreenSpaceMode(bool p_ss_mode)
	{
		ss_mode = p_ss_mode;
	}
	json serialize(vector<vector<char>>* bufs)
	{	
		json data = {
			{"type", "particle"},
			{"tex", tex},
			{"tex_filter_nearest", filter_nearest},
			{"default_color_rgb", toJson(default_color)},
			{"size", size},
			{"ss_mode", ss_mode}
		};
			
		//{"verts", toJson(verts)},
		if (bufs)
		{
			data["verts"] = produceVertDataRaw(verts, bufs);
		}
		else
		{
			data["verts"] = produceVertDataBase64(verts);
		}

		if (colors.rows() > 0)
		{
			//{"colors", toJson(colors)},
			if (bufs)
			{
				data["colors"] = produceVertDataRaw(colors, bufs);
			}
			else
			{
				data["colors"] = produceVertDataBase64(colors);
			}
		}

		return data;
	}
private:
	MatX verts;
	MatX colors;
	string tex;
	bool filter_nearest = false;
	Vec4 default_color = Vec4(1,1,1,1);
	float size = 0.03f;
	bool ss_mode = false;
};

}