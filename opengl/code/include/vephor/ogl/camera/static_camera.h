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

#include "camera_manager.h"

namespace vephor
{

class StaticCamera : public CameraManager
{
public:
	virtual void setup(const json& data, Window& window, AssetManager& assets) override
	{
		bool back_tex_nearest;
		auto back_img = *getCameraBackground(data, back_tex_nearest);
		auto back_tex = window.getTextureFromImage(back_img, back_tex_nearest);
		auto back = make_shared<Background>(back_tex);
		window.add(back, Transform3(), false, -1);
		
		to = readVec3(data["to"]);
		from = readVec3(data["from"]);
		up = readVec3(data["up"]);
		window.setCamFromWorld(makeLookAtTransform(
			to, from, up
		));
	}
	virtual json serialize() override
	{
		return {
			{"type", "static"},
			{"to", toJson(to)},
			{"from", toJson(from)},
			{"up", toJson(up)}
		};
	}
	
	Vec3 to, from, up;
};

}