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