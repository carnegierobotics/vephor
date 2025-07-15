/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#pragma once

#include "camera_manager.h"

namespace vephor
{
namespace ogl
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

} // namespace ogl
} // namespace vephor