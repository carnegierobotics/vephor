#pragma once

#include "window.h"
#include "cylinder.h"

namespace vephor
{

class Axes
{
public:
    Axes(float p_size = 1.0f)
	: size(p_size)
	{}
	json serialize(vector<vector<char>>*)
	{
		return {
			{"type", "axes"},
			{"size", size}
		};
	}
private:
	float size;
};

}