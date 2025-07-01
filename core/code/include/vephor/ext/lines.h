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

class Lines
{
public:
    Lines(const MatXRef& p_verts, const MatXRef& p_colors = MatX())
	: verts(p_verts.transpose()), colors(p_colors.transpose())
	{
	}
	Lines(const vector<Vec3>& p_verts, const vector<Vec4>& p_colors = vector<Vec4>())
	{
		verts = MatXMap(reinterpret_cast<const float*>(p_verts.data()), 3, p_verts.size());
		
		if (!p_colors.empty())
			colors = MatXMap(reinterpret_cast<const float*>(p_colors.data()), 4, p_colors.size());
	}
	void setColor(const Color& p_color){default_color_rgba = p_color.getRGBA();}
	void setLineStrip(bool p_is_strip){strip = p_is_strip;}
	void setAlpha(bool p_is_alpha){alpha = p_is_alpha;}
	json serialize(vector<vector<char>>* bufs)
	{
		json json_data = {
            {"type", "lines"}
        };

		VEPHOR_SERIALIZE_IF_STANDARD(default_color_rgba);
		VEPHOR_SERIALIZE_IF_STANDARD(strip);
		VEPHOR_SERIALIZE_IF_STANDARD(alpha);
		
		//{"verts", toJson(verts)},
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

	inline const static Vec4 default_color_rgba_default = Vec4(1,1,1,1);
	Vec4 default_color_rgba = default_color_rgba_default;

	inline const static bool strip_default = true;
	bool strip = strip_default;

	inline const static bool alpha_default = true;
	bool alpha = alpha_default;
};

}