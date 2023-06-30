#pragma once

#include "window.h"
#include "mesh.h"

namespace vephor
{

class Arrow
{
public:
    Arrow(float p_length = 1.0f, float p_rad = 1.0f, int p_slices = 16)
	: start(0,0,-p_length/2), end(0,0,p_length/2), rad(p_rad), slices(p_slices)
	{}
	Arrow(const Vec3& p_start, const Vec3& p_end, float p_rad = 1.0f, int p_slices = 16)
	: start(p_start), end(p_end), rad(p_rad), slices(p_slices)
	{}
	void setColor(const Vec3& p_color){color = p_color;}
	json serialize(vector<vector<char>>*)
	{
		return {
			{"type", "arrow"},
			{"start", toJson(start)},
			{"end", toJson(end)},
			{"rad", rad},
			{"slices", slices},
			{"color_rgb", toJson(color)}
		};
	}
private:
	Vec3 start;
	Vec3 end;
	float rad;
	float slices;
	Vec3 color = Vec3(1,1,1);
};

}