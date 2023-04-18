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
	virtual void autoFitPoints(Window& window, const vector<Vec3>& pts){}
	virtual void update(Window& window, float dt, const ControlInfo& control_info){}
	virtual json serialize() = 0;

	shared_ptr<Texture> text_tex;
};