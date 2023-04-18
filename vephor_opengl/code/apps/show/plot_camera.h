#pragma once

#include "show_camera.h"

class PlotCamera : public ShowCamera
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
private:
	json serialization;
	bool auto_fit = true;
	string title;
	string x_axis;
	string y_axis;
	bool equal = false;
	bool xflip = false;
	bool yflip = false;
	Vec3 content_min;
	Vec3 content_max;
	Vec3 orig_content_min;
	Vec3 orig_content_max;
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
};