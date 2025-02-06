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

#include "vephor_ogl.h"
#include "asset_manager.h"

using namespace vephor;

struct ControlInfo
{
	bool left_drag_on = false;
	bool right_drag_on = false;
	unordered_map<int, bool> key_down;
	Vec3 key_motion = Vec3::Zero();
	float total_scroll_amount = 0.0f;
	Vec2 drag_start_mouse_pos;
	Transform3 drag_cam_from_world;
};

class ShowCamera
{
public:
	virtual void resizeWindow(Window& window)
	{
		Mat4 proj = makePerspectiveProj(45, window.getSize(), 0.1f, 100.0f);
		window.setProjectionMatrix(proj);
	}
	virtual void setup(const json& data, Window& window, AssetManager& assets) = 0;
	virtual void update(const json& data, Window& window, AssetManager& assets){}
	virtual void autoFitPoints(Window& window, const vector<Vec3>& pts){}
	virtual void update(Window& window, float dt, const ControlInfo& control_info){}
	virtual json serialize() = 0;
	virtual void augmentInputEvent(Window& window, json& event_data){}

	shared_ptr<Texture> text_tex;
};

inline shared_ptr<Image<uint8_t>> getCameraBackground(const json& data, bool& nearest)
{
	nearest = false;

	if (data.contains("background"))
	{
		auto background = data["background"];

		if (background.contains("type"))
		{
			string type = background["type"];
			if (type == "solid")
			{
				Vec3 color = readDefault(background, "color", Vec3(0.5,0.5,0.5));
				return generateSimpleImage(Vec2i(64,64), color);
			}
			else if (type == "gradient")
			{
				Vec3 top_color = readDefault(background, "top", Vec3(0.3,0.3,0.6));
				Vec3 bottom_color = readDefault(background, "bottom", Vec3(0.05,0.05,0.1));
				return generateGradientImage(Vec2i(64,64), top_color, bottom_color);
			}
			else if (type == "checker")
			{
				Vec3 color_1 = readDefault(background, "color_1", Vec3(0.25,0.25,0.25));
				Vec3 color_2 = readDefault(background, "color_2", Vec3(0.75,0.75,0.75));
				Vec2i n_cells = readDefault(background, "n_cells", Vec2i(8,8));

				nearest = true;

				return generateCheckerboardImage(Vec2i(64,64), n_cells, color_1, color_2);
			}
		}
	}

	return generateGradientImage(Vec2i(64,64), Vec3(0.3,0.3,0.6), Vec3(0.05,0.05,0.1));
}