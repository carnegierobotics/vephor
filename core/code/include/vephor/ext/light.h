/**
 * Copyright 2024
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

class AmbientLight
{
public:
    AmbientLight(const Vec3& p_strength):strength(p_strength){}
    json serialize(vector<vector<char>>*)
	{	
		return {
            {"type", "ambient_light"},
			{"strength", toJson(strength)}
        };
	}
private:
    Vec3 strength;
};

class DirLight
{
public:
    DirLight(const Vec3& p_dir, float p_strength=0.5f):dir(p_dir), strength(p_strength){}
    json serialize(vector<vector<char>>*)
	{	
		return {
            {"type", "dir_light"},
			{"dir", toJson(dir)},
			{"strength", strength}
        };
	}
private:
    Vec3 dir;
    float strength;
};

}