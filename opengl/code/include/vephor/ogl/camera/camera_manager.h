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

#include "../window.h"
#include "../asset_manager.h"

namespace vephor
{

struct ControlInfo
{
	bool left_drag_on = false;
	bool right_drag_on = false;
	bool left_press = false;
	bool left_release = false;
	bool right_press = false;
	bool right_release = false;
	unordered_map<int, bool> key_down;
	Vec3 key_motion = Vec3::Zero();
	float total_scroll_amount = 0.0f;
	Vec2 drag_start_mouse_pos;
	Transform3 drag_cam_from_world;

	void onLeftMouseButtonPress(const Window& window)
	{
		Vec2 pos = window.getMousePos();
		drag_start_mouse_pos = pos;
		drag_cam_from_world = window.getCamFromWorld();
		left_drag_on = true;
		left_press = true;
	}
	void onLeftMouseButtonRelease()
	{
		left_drag_on = false;
		left_release = true;
	}
	void onRightMouseButtonPress(const Window& window)
	{
		Vec2 pos = window.getMousePos();
		drag_start_mouse_pos = pos;
		drag_cam_from_world = window.getCamFromWorld();
		right_drag_on = true;
		right_press = true;
	}
	void onRightMouseButtonRelease()
	{
		right_drag_on = false;
		right_release = true;
	}

	void refreshKeyMotion()
	{
		key_motion = Vec3::Zero();
		if (key_down[GLFW_KEY_D])
		{
			key_motion[0] = 1;
		}
		if (key_down[GLFW_KEY_A])
		{
			key_motion[0] = -1;
		}
		if (key_down[GLFW_KEY_W])
		{
			key_motion[1] = 1;
		}
		if (key_down[GLFW_KEY_S])
		{
			key_motion[1] = -1;
		}
		if (key_down[GLFW_KEY_F])
		{
			key_motion[2] = 1;
		}
		if (key_down[GLFW_KEY_R])
		{
			key_motion[2] = -1;
		}
	}
	void onKeyPress(int key)
	{
		key_down[key] = true;
		refreshKeyMotion();
	}
	void onKeyRelease(int key)
	{
		key_down[key] = false;
		refreshKeyMotion();
	}

	void onScroll(float amount)
	{
		total_scroll_amount += amount;
	}

	void onUpdate()
	{
		total_scroll_amount = 0.0f;
		left_press = false;
		left_release = false;
		right_press = false;
		right_release = false;
	}
};

class BoundManager
{
public:
	BoundManager(const Window* p_window)
	: window(p_window)
	{}
	void addBoundPoint(const Vec3& pt, const TransformSim3& world_from_body)
	{
		auto world_pt = world_from_body * pt;
		auto cam_pt = window->getCamFromWorld() * world_pt;
		
		for (int i = 0; i < 3; i++)		
		{
			if (world_pt[i] < object_bound_in_world_min[i])
				object_bound_in_world_min[i] = world_pt[i];
			if (world_pt[i] > object_bound_in_world_max[i])
				object_bound_in_world_max[i] = world_pt[i];
		}
	}
	void addBoundVerts(const MatXRef& verts, const TransformSim3& world_from_body)
	{
		if (verts.rows() == 0)
			return;

		Vec3 min, max;
		for (int i = 0; i < 3; i++)
		{
			min[i] = verts.col(i).minCoeff();
			max[i] = verts.col(i).maxCoeff();
		}
		
		addBoundPoint(min, world_from_body);
		addBoundPoint(Vec3(min[0],min[1],max[2]), world_from_body);
		addBoundPoint(Vec3(min[0],max[1],min[2]), world_from_body);
		addBoundPoint(Vec3(max[0],min[1],min[2]), world_from_body);
		addBoundPoint(Vec3(max[0],max[1],min[2]), world_from_body);
		addBoundPoint(Vec3(max[0],min[1],max[2]), world_from_body);
		addBoundPoint(Vec3(min[0],max[1],max[2]), world_from_body);
		addBoundPoint(max, world_from_body);
	}
	void addBoundSphere(float rad, const TransformSim3& world_from_body)
	{
		addBoundPoint(Vec3(-rad,0,0), world_from_body);
		addBoundPoint(Vec3(rad,0,0), world_from_body);
		addBoundPoint(Vec3(0,-rad,0), world_from_body);
		addBoundPoint(Vec3(0,rad,0), world_from_body);
		addBoundPoint(Vec3(0,0,-rad), world_from_body);
		addBoundPoint(Vec3(0,0,rad), world_from_body);
	}
	vector<Vec3> calcCameraBoundPoints()
	{
		if (object_bound_in_world_min[0] > object_bound_in_world_max[0])
			return {};
		
		vector<Vec3> camera_bound_points = {
			Vec3(object_bound_in_world_min[0], object_bound_in_world_min[1], object_bound_in_world_min[2]),
			Vec3(object_bound_in_world_min[0], object_bound_in_world_min[1], object_bound_in_world_max[2]),
			Vec3(object_bound_in_world_min[0], object_bound_in_world_max[1], object_bound_in_world_min[2]),
			Vec3(object_bound_in_world_max[0], object_bound_in_world_min[1], object_bound_in_world_min[2]),
			Vec3(object_bound_in_world_max[0], object_bound_in_world_max[1], object_bound_in_world_min[2]),
			Vec3(object_bound_in_world_max[0], object_bound_in_world_min[1], object_bound_in_world_max[2]),
			Vec3(object_bound_in_world_min[0], object_bound_in_world_max[1], object_bound_in_world_max[2]),
			Vec3(object_bound_in_world_max[0], object_bound_in_world_max[1], object_bound_in_world_max[2])
		};

		return camera_bound_points;
	}
private:
	const Window* window = NULL;

	Vec3 object_bound_in_world_min = Vec3(
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max()
	);
	Vec3 object_bound_in_world_max = Vec3(
		std::numeric_limits<float>::lowest(),
		std::numeric_limits<float>::lowest(),
		std::numeric_limits<float>::lowest()
	);
};

struct SelectionWidget
{
	Vec3 center;
	float radius;
	shared_ptr<RenderNode> node;
};

class CameraManager
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
	virtual void addSelectionWidget(const Vec3& center, float radius, shared_ptr<RenderNode>& node){}
	
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

}