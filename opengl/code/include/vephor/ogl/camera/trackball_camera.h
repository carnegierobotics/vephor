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
#include "../draw/background.h"
#include "../draw/mesh.h"

namespace vephor
{

class TrackballCamera : public CameraManager
{
public:
	virtual void resizeWindow(Window& window) override;
	virtual void setup(const json& data, Window& window, AssetManager& assets) override;
	virtual void autoFitPoints(Window& window, const vector<Vec3>& pts) override;
	virtual void update(Window& window, float dt, const ControlInfo& control_info) override;
	virtual json serialize() override
	{
		return {
			{"type", "trackball"},
			{"to", toJson(trackball_to)},
			{"from", toJson(trackball_from)},
			{"up", toJson(trackball_up)},
			{"3d", trackball_3d},
			{"scene_scale", scene_scale},
			{"auto_fit", false}
		};
	}
private:
	float scene_scale = 12.0f;
	shared_ptr<RenderNode> orbit_point_render;
	shared_ptr<RenderNode> drag_point_render;
	const float orbit_point_scene_scale_mult = 1.0f / 100.0f;
	bool trackball_3d = true;
	Vec3 trackball_to;
	Vec3 trackball_from;
	Vec3 trackball_up;
	Vec3 trackball_fore;
	Vec3 trackball_right;
	float fov = 45.0f;
	float curr_near_z = 0.1f;
	float curr_far_z = 100.0f;
	bool auto_fit = true;

	bool right_drag_off = true;
	Vec3 right_world_point;
};

}