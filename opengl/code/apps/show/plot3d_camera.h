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

class Plot3DCamera : public ShowCamera
{
public:
	virtual void resizeWindow(Window& window) override;
	virtual void setup(const json& data, Window& window, AssetManager& assets) override;
	virtual void autoFitPoints(Window& window, const vector<Vec3>& pts) override;
	virtual void update(Window& window, float dt, const ControlInfo& control_info) override;
	virtual json serialize() override
	{
		return {
			{"type", "plot3d"},
			{"to", toJson(trackball_to)},
			{"from", toJson(trackball_from)},
			{"up", toJson(trackball_up)},
			{"3d", trackball_3d},
			{"scene_scale", scene_scale},
			{"auto_fit", false}
		};
	}
private:
	void moveGrid(Window& window);

	float scene_scale = 12.0f;
	shared_ptr<RenderNode> orbit_point_render;
	shared_ptr<RenderNode> drag_point_render;
	shared_ptr<RenderNode> grid_render;
	shared_ptr<RenderNode> axes_render;
	shared_ptr<RenderNode> zero_axes_render;
	shared_ptr<RenderNode> curr_pos_text_render;
	shared_ptr<RenderNode> curr_inc_text_render;
	const float orbit_point_scene_scale_mult = 1.0f / 100.0f;
	bool trackball_3d = false;
	Vec3 trackball_to;
	Vec3 trackball_from;
	Vec3 trackball_up;
	Vec3 trackball_fore;
	Vec3 trackball_right;
	float fov = 45.0f;
	float curr_near_z = 0.1f;
	float curr_far_z = 100.0f;
	bool auto_fit = true;
	float zoom = 40.0f;

	bool right_drag_off = true;
	Vec3 right_world_point;
};