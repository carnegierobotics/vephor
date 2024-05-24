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

#include "show_camera.h"

class StaticCamera : public ShowCamera
{
public:
	virtual void setup(const json& data, Window& window, AssetManager& assets) override
	{
		auto back_tex = window.getTextureFromImage(*generateGradientImage(Vec2i(64,64), Vec3(0.3,0.3,0.6), Vec3(0.05,0.05,0.1)));
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