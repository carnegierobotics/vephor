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
	void setColor(const Color& p_color){default_color = p_color.getRGBA();}
	void setLineStrip(bool p_is_strip){is_strip = p_is_strip;}
	json serialize(vector<vector<char>>* bufs)
	{
		json data = {
            {"type", "lines"},
			{"default_color_rgba", toJson(default_color)},
			{"strip", is_strip},
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
	Vec4 default_color = Vec4(1,1,1,1);
	bool is_strip = true;
};

}