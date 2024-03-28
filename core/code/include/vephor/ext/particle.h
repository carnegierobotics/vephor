/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Carnegie Robotics, LLC nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CARNEGIE ROBOTICS, LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
	void setColor(const Color& p_color){default_color = p_color.getRGBA();}
	void setTexture(const string& p_tex, bool p_filter_nearest = false)
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