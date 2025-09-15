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
#include "../draw/lines.h"
#include "../draw/mesh.h"
#include "../draw/text.h"
#include "../draw/sprite.h"
#include "../draw/instanced_points.h"
#include "../draw/background.h"
#include "../draw/light.h"

namespace vephor
{
namespace ogl
{

class PlotCamera : public CameraManager
{
public:
	void calcEqualContentBounds(
		Window& window, 
		Vec3& curr_inner_content_min, 
		Vec3& curr_inner_content_max,
		Vec3& curr_content_min, 
		Vec3& curr_content_max);
	virtual void resizeWindow(Window& window) override;
	virtual void setup(const json& data, Window& window, AssetManager& assets) override;
	virtual void update(const json& data, Window& window, AssetManager& assets) override;
	virtual void autoFitPoints(Window& window, const vector<Vec3>& pts) override;
	float findBestTickRes(float span, float window_span);
	virtual void update(Window& window, float dt, const ControlInfo& control_info) override;
	virtual json serialize() override
	{
		serialization["auto_fit"] = false;
		serialization["content_min"] = toJson(content_min);
		serialization["content_max"] = toJson(content_max);
		serialization["orig_content_min"] = toJson(orig_content_min);
		serialization["orig_content_max"] = toJson(orig_content_max);
		return serialization;
	}
	virtual void augmentInputEvent(Window& window, json& event_data) override;
private:
	void setupTitle(const string& title, Window& window);
	void setupLegend(const json& label_data, Window& window, AssetManager& assets);

	json serialization;
	bool auto_fit = true;
	string title;
	string x_axis;
	string y_axis;
	Vec3 back_color;
	Vec3 fore_color;
	Vec3 grid_color;
	bool equal = false;
	bool xflip = false;
	bool yflip = false;
	bool xyswap = false;
	bool cursor_callout = true;
	bool legend_right = true;
	bool legend_top = true;
	Vec3 content_min = Vec3(-0.5,-0.5,-1);
	Vec3 content_max = Vec3(0.5,0.5,1e6);
	Vec3 orig_content_min = Vec3(-0.5,-0.5,-1);
	Vec3 orig_content_max = Vec3(0.5,0.5,1e6);
	Vec3 pre_drag_content_min;
	Vec3 pre_drag_content_max;
	bool last_left_drag_on = false;
	shared_ptr<RenderNode> plot_box;
	shared_ptr<RenderNode> plot_box_fill;
	shared_ptr<RenderNode> plot_title;
	shared_ptr<RenderNode> x_axis_title;
	shared_ptr<RenderNode> y_axis_title;
	float box_border;
	float box_left_border;
	float box_bottom_border;
	float inner_border;
	float text_scale;
	float tick_size;
	float tick_dist;
	shared_ptr<Text> title_text;
	shared_ptr<Text> mouse_pos_text;
	shared_ptr<RenderNode> mouse_pos_node;
	shared_ptr<RenderNode> legend_node;
	vector<shared_ptr<RenderNode>> vert_grid_lines;
	vector<shared_ptr<RenderNode>> horiz_grid_lines;
	vector<shared_ptr<RenderNode>> vert_lines;
	vector<shared_ptr<RenderNode>> horiz_lines;
	vector<shared_ptr<Text>> vert_text;
	vector<shared_ptr<Text>> horiz_text;
	vector<shared_ptr<RenderNode>> vert_text_node;
	vector<shared_ptr<RenderNode>> horiz_text_node;
	vector<shared_ptr<RenderNode>> marked_position_nodes;
	bool key_m_down = false;
	bool key_n_down = false;
	shared_ptr<Texture> circle_tex;
};

} // namespace ogl
} // namespace vephor