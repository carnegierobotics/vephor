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
#include "mesh.h"

namespace vephor
{
	
class Cube
{
public:
    Cube(float p_rad=1.0f)
	: rad(p_rad)
	{}
	void setColor(const Color& p_color){color = p_color.getRGB();}
	json serialize(vector<vector<char>>*)
	{	
		return {
            {"type", "cube"},
			{"rad", rad},
			{"color_rgb", toJson(color)}
        };
	}
private:
	float rad;
	Vec3 color = Vec3(1,1,1);
};

}