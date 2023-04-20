#include "plot_camera.h"

void PlotCamera::calcEqualContentBounds(
	Window& window, 
	Vec3& curr_inner_content_min, 
	Vec3& curr_inner_content_max,
	Vec3& curr_content_min, 
	Vec3& curr_content_max)
{
	curr_inner_content_min = content_min;
	curr_inner_content_max = content_max;
	
	Vec2 inner_window_span(
		window.getSize()[0] - (box_left_border + inner_border) - (box_border + inner_border),
		window.getSize()[1] - (box_bottom_border + inner_border) - (box_border + inner_border)
	);
	
	if (equal)
	{
		float inner_window_aspect = inner_window_span[0] / inner_window_span[1];
		
		Vec3 span = curr_inner_content_max - curr_inner_content_min;
		Vec3 center = (curr_inner_content_min + curr_inner_content_max) / 2.0f;
		
		float bound_aspect = span[0] / span[1];
		
		if (bound_aspect < inner_window_aspect)
		{
			span[0] = span[1] * inner_window_aspect;
		}
		else
		{
			span[1] = span[0] / inner_window_aspect;
		}
		
		curr_inner_content_min = center - span / 2.0f;
		curr_inner_content_max = center + span / 2.0f;
	}
	
	curr_content_min = curr_inner_content_min;
	curr_content_max = curr_inner_content_max;
	
	if (xflip)
		std::swap(curr_content_min[0], curr_content_max[0]);
	if (yflip)
		std::swap(curr_content_min[1], curr_content_max[1]);
	
	Vec3 span = curr_content_max - curr_content_min;
	Vec3 center = (curr_content_min + curr_content_max) / 2.0f;
	
	curr_content_min[0] = curr_content_min[0] - span[0] * (box_left_border + inner_border) / inner_window_span[0];
	curr_content_max[0] = curr_content_max[0] + span[0] * (box_border + inner_border) / inner_window_span[0];
	curr_content_min[1] = curr_content_min[1] - span[1] * (box_bottom_border + inner_border) / inner_window_span[1];
	curr_content_max[1] = curr_content_max[1] + span[1] * (box_border + inner_border) / inner_window_span[1];
}

void PlotCamera::resizeWindow(Window& window)
{
	Vec3 curr_content_inner_min;
	Vec3 curr_content_inner_max;
	Vec3 curr_content_min;
	Vec3 curr_content_max;
	
	calcEqualContentBounds(window, 
		curr_content_inner_min, 
		curr_content_inner_max,
		curr_content_min, 
		curr_content_max);

	float far_z = -curr_content_max[2];
	float near_z = -curr_content_min[2];

	curr_content_min[2] = far_z;
	curr_content_max[2] = near_z;
		
	Mat4 proj = makeOrthoProj(curr_content_min, curr_content_max);
	//proj(2,2) *= -1;
	window.setProjectionMatrix(proj);
	
	Vec4 proj_min_pt = proj.transpose() * Vec4(0,0,curr_content_min[2],1);
	Vec4 proj_max_pt = proj.transpose() * Vec4(0,0,curr_content_max[2],1);
	proj_min_pt /= proj_min_pt[3];
	proj_max_pt /= proj_max_pt[3];
	
	if (plot_box)
		plot_box->setDestroy();
	
	if (plot_box_fill)
		plot_box_fill->setDestroy();

	float box_right_border = window.getSize()[0]-box_border;
	float box_top_border = window.getSize()[1]-box_border;

	MatX verts(5,3);
	verts << 
		box_left_border,box_bottom_border,0,
		box_left_border,box_top_border,0,
		box_right_border,box_top_border,0,
		box_right_border,box_bottom_border,0,
		box_left_border,box_bottom_border,0;
	auto plot_box_lines = make_shared<Lines>(verts, MatX(), Vec4(0,0,0,1));
	plot_box = window.add(plot_box_lines, TransformSim3(), true);
	
	MeshData border_fill;
	border_fill.verts.resize(8*3,3);
	border_fill.norms.resize(8*3,3);
	border_fill.uvs.resize(8*3,2);
	border_fill.addQuad2D(
		Vec2(0,0),
		Vec2(window.getSize()[0],box_bottom_border),
		Vec2(0,0),
		Vec2(1,1));
	border_fill.addQuad2D(
		Vec2(0,box_top_border),
		Vec2(window.getSize()[0],window.getSize()[1]),
		Vec2(0,0),
		Vec2(1,1));
	border_fill.addQuad2D(
		Vec2(0,box_bottom_border),
		Vec2(box_left_border,box_top_border),
		Vec2(0,0),
		Vec2(1,1));
	border_fill.addQuad2D(
		Vec2(box_right_border,box_bottom_border),
		Vec2(window.getSize()[0],box_top_border),
		Vec2(0,0),
		Vec2(1,1));
	plot_box_fill = window.add(make_shared<Mesh>(border_fill, Vec3(1,1,1), 0.0, 0.0, 1.0), TransformSim3(), true);
	
	if (plot_title)
	{
		Vec3 pos(window.getSize()[0]/2,window.getSize()[1] - text_scale,-1);
		plot_title->setPos(pos);
	}

	if (x_axis_title)
	{
		Vec3 pos((box_left_border+box_right_border)/2.0f,text_scale*2.0f/3.0f,-1);
		x_axis_title->setPos(pos);
	}

	if (y_axis_title)
	{
		Vec3 pos(text_scale*2.0f/3.0f,(box_bottom_border+box_top_border)/2.0f,-1);
		y_axis_title->setPos(pos);
	}
}

void PlotCamera::setup(const json& data, Window& window, AssetManager& assets)
{	
	serialization = data;

	circle_tex = window.loadTexture(assets.getAssetPath("/assets/circle.png"), false);

	box_border = 30.0f * window.getContentScale()[1];
	box_left_border = 100.0f * window.getContentScale()[1];
	box_bottom_border = 60.0f * window.getContentScale()[1];
	inner_border = 15.0f * window.getContentScale()[1];
	text_scale = 15.0f * window.getContentScale()[1];
	tick_size = 10.0f * window.getContentScale()[1];
	tick_dist = 100.0f * window.getContentScale()[1];

	if (data.contains("title"))
		title = data["title"];

	if (data.contains("x_axis"))
		x_axis = data["x_axis"];

	if (data.contains("y_axis"))
		y_axis = data["y_axis"];

	auto back_tex = window.getTextureFromImage(*generateSimpleImage(Vec2i(64,64), Vec3(1,1,1)));
	auto back = make_shared<Background>(back_tex);
	window.add(back, Transform3(), false, -1);

	if (data.contains("equal"))
		equal = data["equal"];
	
	if (data.contains("x_flip"))
		xflip = data["x_flip"];
	
	if (data.contains("y_flip"))
		yflip = data["y_flip"];
	
	mouse_pos_text = make_shared<Text>("", text_tex, Vec3(0,0,0));
	mouse_pos_node = window.add(mouse_pos_text, TransformSim3(Vec3(0,0,-1)), true, 1);
	mouse_pos_node->setScale(text_scale);
	
	if (!title.empty())
	{
		title_text = make_shared<Text>(title, text_tex, Vec3(0,0,0));
		title_text->setAnchorCentered();
		
		plot_title = window.add(
			title_text, 
			TransformSim3(), 
			true,
			1);
		plot_title->setScale(text_scale);
	}

	if (!x_axis.empty())
	{
		auto text = make_shared<Text>(x_axis, text_tex, Vec3(0,0,0));
		text->setAnchorBottom();

		x_axis_title = window.add(
			text, 
			TransformSim3(), 
			true,
			1);
		x_axis_title->setScale(text_scale);
	}

	if (!y_axis.empty())
	{
		auto text = make_shared<Text>(y_axis, text_tex, Vec3(0,0,0));
		text->setAnchorTop();
		text->setBillboard(false);

		y_axis_title = window.add(
			text, 
			TransformSim3(), 
			true,
			1);
		y_axis_title->setOrient(Vec3(0,0,M_PI/2.0f));
		y_axis_title->setScale(text_scale);
	}
	
	const size_t max_num_ticks = 50;
	const float grid_shade = 0.9;
	
	{
		MatX verts(2,3);
		verts << 0,0,0,
				 0,-tick_size,0;
		auto line = make_shared<Lines>(verts,MatX(),Vec4(0,0,0,1));

		MatX grid_verts(2,3);
		grid_verts << 0,-0.5,0,
				 0,0.5,0;
		auto grid_line = make_shared<Lines>(grid_verts,MatX(),Vec4(grid_shade,grid_shade,grid_shade,1));
		
		for (size_t i = 0; i < max_num_ticks; i++)
		{
			vert_lines.push_back(window.add(
				line,
				TransformSim3(),
				true,
				1
			));
			(*vert_lines.rbegin())->setShow(false);

			vert_grid_lines.push_back(window.add(
				grid_line,
				TransformSim3()
			));
			(*vert_grid_lines.rbegin())->setShow(false);
			
			vert_text.push_back(make_shared<Text>("", text_tex, Vec3(0,0,0)));
			(*vert_text.rbegin())->setAnchorTop();
			vert_text_node.push_back(window.add(*vert_text.rbegin(), TransformSim3(), true, 1));
			(*vert_text_node.rbegin())->setScale(text_scale);
			(*vert_text_node.rbegin())->setShow(false);
		}
	}
	
	{
		MatX verts(2,3);
		verts << 0,0,0,
				 -tick_size,0,0;
		auto line = make_shared<Lines>(verts,MatX(),Vec4(0,0,0,1));

		MatX grid_verts(2,3);
		grid_verts << -0.5,0,0,
				 0.5,0,0;
		auto grid_line = make_shared<Lines>(grid_verts,MatX(),Vec4(grid_shade,grid_shade,grid_shade,1));
		
		for (size_t i = 0; i < max_num_ticks; i++)
		{
			horiz_lines.push_back(window.add(
				line,
				TransformSim3(),
				true,
				1
			));
			(*horiz_lines.rbegin())->setShow(false);

			horiz_grid_lines.push_back(window.add(
				grid_line,
				TransformSim3()
			));
			(*horiz_grid_lines.rbegin())->setShow(false);
			
			horiz_text.push_back(make_shared<Text>("", text_tex, Vec3(0,0,0)));
			(*horiz_text.rbegin())->setAnchorRight();
			horiz_text_node.push_back(window.add(*horiz_text.rbegin(), TransformSim3(), true, 1));
			(*horiz_text_node.rbegin())->setScale(text_scale);
			(*horiz_text_node.rbegin())->setShow(false);
		}
	}
	
	if (data.contains("labels"))
	{
		v4print "Legend";
		legend_node = window.add(TransformSim3());
		
		vector<shared_ptr<RenderNode>> legend_text_nodes;
		
		float x_max = 0.0f;
		float y_shift = 0.0f;
		for (const auto& label : data["labels"])
		{
			v4print "Label:", label["text"];
			auto text = make_shared<Text>(label["text"], text_tex, Vec3(0,0,0));
			text->setAnchorTopLeft();
			auto node = window.add(text, Vec3(0, y_shift, 0), true, 2);
			node->setScale(text_scale);
			node->setParent(legend_node);
			legend_text_nodes.push_back(node);
			
			Vec2 text_size = text->getSize() * text_scale;
			if (text_size[0] > x_max)
				x_max = text_size[0];
			y_shift -= text_size[1];
		}
		
		for (auto& node : legend_text_nodes)
		{
			Vec3 pos = node->getPos();
			pos[0] = -x_max;
			node->setPos(pos);
		}
		
		// Make room for icons
		x_max += text_scale * 2;
		
		for (size_t i = 0; i < data["labels"].size(); i++)
		{
			const auto& label = data["labels"][i];
			if (label["type"] == "line")
			{
				MatX verts(2,3);
				verts << -text_scale*0.75,0,0,text_scale*0.75,0,0;
				Vec4 color;
				color.head<3>() = readVec3(label["color"]);
				color[3] = 1.0f;
				auto node = window.add(
					make_shared<Lines>(verts, MatX(), color),
					TransformSim3(Vec3(-x_max + text_scale,legend_text_nodes[i]->getPos()[1]-text_scale/2.0f,0)),
					true,
					2
				);
				node->setParent(legend_node);
			}
			else
			{
				shared_ptr<Texture> marker_tex; 
				
				if (label["type"] == "circle")
					marker_tex = window.loadTexture(assets.getAssetPath("/assets/circle.png"));
				else if (label["type"] == "diamond")
					marker_tex = window.loadTexture(assets.getAssetPath("/assets/diamond.png"));
				else if (label["type"] == "plus")
					marker_tex = window.loadTexture(assets.getAssetPath("/assets/plus.png"));
				else if (label["type"] == "square")
					marker_tex = window.getTextureFromImage(*generateSimpleImage(Vec2i(8,8), Vec3(1,1,1)));
				else
					throw std::runtime_error("Invalid marker type: " + string(label["type"]));
				
				auto marker_sprite = make_shared<Sprite>(marker_tex);
				marker_sprite->setDiffuse(Vec3::Zero());
				marker_sprite->setAmbient(Vec3::Zero());
				marker_sprite->setEmissive(readVec3(label["color"]));
				
				auto node = window.add(
					marker_sprite,
					TransformSim3(Vec3(-x_max + text_scale,legend_text_nodes[i]->getPos()[1]-text_scale/2.0f,0)),
					true,
					2
				);
				node->setScale(text_scale*0.5f);
				node->setParent(legend_node);
			}
		}
		
		MatX legend_box_verts(5,3);
		legend_box_verts <<
			0,0,0,
			-x_max,0,0,
			-x_max,y_shift,0,
			0,y_shift,0,
			0,0,0;
		auto legend_box = make_shared<Lines>(legend_box_verts,MatX(),Vec4(0,0,0,1));
		window.add(legend_box, TransformSim3(), true, 1)->setParent(legend_node);
		
		MeshData legend_fill_verts;
		legend_fill_verts.verts.resize(2*3,3);
		legend_fill_verts.norms.resize(2*3,3);
		legend_fill_verts.uvs.resize(2*3,2);
		legend_fill_verts.addQuad2D(
			Vec2(-x_max,y_shift), 
			Vec2(0,0),
			Vec2(0,0),
			Vec2(1,1)
		);
		auto legend_fill = make_shared<Mesh>(legend_fill_verts, Vec3(1,1,1), 0, 0, 1);
		window.add(legend_fill, TransformSim3(), true, 1)->setParent(legend_node);
		
		Vec3 pos;
		pos.head<2>() = Vec2(-box_border - text_scale, -box_border - text_scale);
		pos[2] = 0;
		legend_node->setPos(pos);
		legend_node->setParent(window.getWindowTopRightNode());
	}
	
	if (data.contains("content_min"))
	{
		content_min = readVec3(data["content_min"]);
		content_min[2] = -1;
	}
	if (data.contains("content_max"))
	{
		content_max = readVec3(data["content_max"]);
		content_max[2] = 1000;
	}
	if (data.contains("orig_content_min"))
	{
		orig_content_min = readVec3(data["orig_content_min"]);
		orig_content_min[2] = -1;
	}
	if (data.contains("orig_content_max"))
	{
		orig_content_max = readVec3(data["orig_content_max"]);
		orig_content_max[2] = 1000;
	}
	
	if (data.contains("auto_fit"))
		auto_fit = data["auto_fit"];
	
	resizeWindow(window);
}

void PlotCamera::autoFitPoints(Window& window, const vector<Vec3>& pts)
{
	if (!auto_fit)
		return;
	
	content_min = pts[0];
	content_max = pts[0];
	for (const auto& pt : pts)
	{
		for (int i = 0; i < 3; i++)
		{
			if (pt[i] < content_min[i])
				content_min[i] = pt[i];
			if (pt[i] > content_max[i])
				content_max[i] = pt[i];
		}
	}

	if (content_min[0] == content_max[0])
	{
		content_min[0] -= 0.5f;
		content_max[0] += 0.5f;
	}
	if (content_min[1] == content_max[1])
	{
		content_min[1] -= 0.5f;
		content_max[1] += 0.5f;
	}
	
	content_min[2] = -1;
	content_max[2] = 1000;
	resizeWindow(window);
	
	orig_content_min = content_min;
	orig_content_max = content_max;
}

float PlotCamera::findBestTickRes(float span, float window_span)
{
	int num_ticks = window_span / tick_dist;
	if (num_ticks < 2)
		num_ticks = 2;
	
	float tick_hint = span / num_ticks;

	float res_power = log(tick_hint) / log(10.0f);
	float res_pow_10 = pow(10, floor(res_power));
	vector<float> res_setpoints = {res_pow_10, 2.5f*res_pow_10, 5.0f*res_pow_10, 10.0f*res_pow_10};

	//cout << "Res: " << tick_hint << endl;
	//cout << "Res power: " << res_power << endl;

	float best_dist = numeric_limits<float>::max();
	float best_res = span;
	for (const auto& p : res_setpoints)
	{
		float dist = fabs(p - tick_hint);
		if (dist < best_dist)
		{
			best_dist = dist;
			best_res = p;
		}
	}

	//cout << "Tick res: " << best_res << endl;

	if (best_res < 1e-6)
		best_res = 1e-6;

	return best_res;
}

void PlotCamera::update(Window& window, float dt, const ControlInfo& control_info)
{
	if (find(control_info.key_down, GLFW_KEY_H) && control_info.key_down.at(GLFW_KEY_H))
	{
		content_min = orig_content_min;
		content_max = orig_content_max;
		resizeWindow(window);
		last_left_drag_on = false;
	}
	else
		if (control_info.left_drag_on || 
			control_info.right_drag_on || 
			control_info.total_scroll_amount != 0.0f || 
			control_info.key_motion != Vec3::Zero())
	{
		Vec3 curr_content_inner_min;
		Vec3 curr_content_inner_max;
		Vec3 curr_content_min;
		Vec3 curr_content_max;
		
		calcEqualContentBounds(window, 
			curr_content_inner_min, 
			curr_content_inner_max,
			curr_content_min, 
			curr_content_max);
		
		if (!last_left_drag_on)
		{
			pre_drag_content_min = content_min;
			pre_drag_content_max = content_max;
		}
		
		Vec2 mouse_delta = window.getMousePos() - control_info.drag_start_mouse_pos;
		if (control_info.total_scroll_amount != 0.0f)
		{
			mouse_delta = Vec2(control_info.total_scroll_amount, control_info.total_scroll_amount) * 20;
		}
		
		if (
			(find(control_info.key_down, GLFW_KEY_LEFT_SHIFT) && control_info.key_down.at(GLFW_KEY_LEFT_SHIFT)) || 
			(find(control_info.key_down, GLFW_KEY_RIGHT_SHIFT) && control_info.key_down.at(GLFW_KEY_RIGHT_SHIFT)) ||
			control_info.right_drag_on ||
			control_info.total_scroll_amount != 0.0f
			)
		{
			Vec2 center = (pre_drag_content_min + pre_drag_content_max).head<2>() / 2.0f;
			Vec2 span = (pre_drag_content_max - pre_drag_content_min).head<2>();
			
			if (equal)
			{
				float mult = exp(-mouse_delta[0]/100.0f) * exp(-mouse_delta[1]/100.0f);

				span = Vec2(
					span[0] * mult,
					span[1] * mult
				);
			}
			else
			{
				span = Vec2(
					span[0] * exp(-mouse_delta[0]/100.0f),
					span[1] * exp(-mouse_delta[1]/100.0f)
				);
			}
			
			if (span[0] < 1e-5)
				span[0] = 1e-5;
			if (span[1] < 1e-5)
				span[1] = 1e-5;
			
			content_min.head<2>() = center - span / 2.0f;
			content_max.head<2>() = center + span / 2.0f;
		}
			else if (control_info.key_motion != Vec3::Zero())
			{
				Vec2 world_per_px(
					(curr_content_max[0] - curr_content_min[0])/(float)window.getSize()[0], 
					(curr_content_max[1] - curr_content_min[1])/(float)window.getSize()[1]
				);
				Vec2 content_delta = (-control_info.key_motion.head<2>().array() * world_per_px.array()) * 20;
				content_min.head<2>() = pre_drag_content_min.head<2>() - content_delta;
				content_max.head<2>() = pre_drag_content_max.head<2>() - content_delta;
			}
		else
		{
			Vec2 world_per_px(
				(curr_content_max[0] - curr_content_min[0])/(float)window.getSize()[0], 
				(curr_content_max[1] - curr_content_min[1])/(float)window.getSize()[1]
			);
			Vec2 content_delta = (mouse_delta.array() * world_per_px.array());
			content_min.head<2>() = pre_drag_content_min.head<2>() - content_delta;
			content_max.head<2>() = pre_drag_content_max.head<2>() - content_delta;
		}
		
		resizeWindow(window);
		
		if (control_info.left_drag_on || control_info.right_drag_on)
			last_left_drag_on = true;
	}
	else
	{
		last_left_drag_on = false;
	}


	auto mouse_pos = window.getMousePos();
	auto window_size = window.getSize();
	
	Vec3 curr_content_inner_min;
	Vec3 curr_content_inner_max;
	Vec3 curr_content_min;
	Vec3 curr_content_max;
	
	calcEqualContentBounds(window, 
		curr_content_inner_min, 
		curr_content_inner_max,
		curr_content_min, 
		curr_content_max);

	Vec2 inner_window_span(
		window.getSize()[0] - (box_left_border + inner_border) - (box_border + inner_border),
		window.getSize()[1] - (box_bottom_border + inner_border) - (box_border + inner_border)
	);
		
	Vec2 tick_res(
		findBestTickRes(curr_content_inner_max[0] - curr_content_inner_min[0], inner_window_span[0]),
		findBestTickRes(curr_content_inner_max[1] - curr_content_inner_min[1], inner_window_span[1])
	);
	
	Vec2 pos(
		mouse_pos[0] / (float)window_size[0] * (curr_content_max[0] - curr_content_min[0]) + curr_content_min[0],
		mouse_pos[1] / (float)window_size[1] * (curr_content_max[1] - curr_content_min[1]) + curr_content_min[1]
	);
	
	mouse_pos_text->setText(to_string(pos[0]) + ", " + to_string(pos[1]));

	if (find(control_info.key_down, GLFW_KEY_M))
	{
		if (control_info.key_down.at(GLFW_KEY_M))
			key_m_down = true;
		else
		if (key_m_down)
		{
			v4print "Marked location:", pos.transpose();
			
			MatX points(1, 3);
			points.setZero();

			auto marked_pos = make_shared<InstancedPoints>(points, MatX(), Vec4(1,0.5,0,1));
			marked_pos->setTexture(circle_tex);
			marked_pos->setScreenSpaceMode(true);
			marked_pos->setSize(0.01);
			marked_position_nodes.push_back(window.add(marked_pos, Vec3(pos[0], pos[1], 999.99)));

			key_m_down = false;
		}
	}

	if (find(control_info.key_down, GLFW_KEY_N))
	{
		if (control_info.key_down.at(GLFW_KEY_N))
			key_n_down = true;
		else
		if (key_n_down)
		{
			if (!marked_position_nodes.empty())
			{
				json marks;

				auto save_dir = getSaveDir();

				v4print "Clearing and saving marks to", save_dir;
				for (auto& node : marked_position_nodes)
				{
					marks.push_back(toJson((Vec2)node->getPos().head<2>()));
					node->setDestroy();
				}

				marked_position_nodes.clear();
				
				ofstream fout(save_dir+"/marks.json");
				fout << marks;
				fout.close();
			}

			key_n_down = false;
		}
	}

	{
		int tick_index = curr_content_inner_min[0] / tick_res[0];
		int tick_line_index = 0;
		
		for (auto& tick_node : vert_lines)
			tick_node->setShow(false);

		for (auto& tick_node : vert_grid_lines)
			tick_node->setShow(false);
		
		for (auto& text_node : vert_text_node)
			text_node->setShow(false);
		
		while (true)
		{
			float value = tick_index * tick_res[0];
			if (value < curr_content_inner_min[0])
			{
				tick_index++;
				continue;
			}
			
			if (value > curr_content_inner_max[0])
				break;
			
			Vec3 pos(
				window.getSize()[0] * (value - curr_content_min[0]) / (curr_content_max[0] - curr_content_min[0]), 
				box_bottom_border,
				-1);
			vert_lines[tick_line_index]->setPos(pos);
			vert_lines[tick_line_index]->setShow(true);

			vert_grid_lines[tick_line_index]->setPos(Vec3(
				value,
				(curr_content_min[1] + curr_content_max[1])/2.0f,
				curr_content_min[2] + 0.01));
			vert_grid_lines[tick_line_index]->setScale(curr_content_max[1] - curr_content_min[1]);
			vert_grid_lines[tick_line_index]->setShow(true);
			
			char tick_text[128];
			sprintf(tick_text, "%.7g", value);
			vert_text[tick_line_index]->setText(tick_text);
			
			vert_text_node[tick_line_index]->setPos(pos-Vec3(0,tick_size,0));
			vert_text_node[tick_line_index]->setShow(true);
			
			tick_line_index++;
			tick_index++;
		}
	}
	
	{
		int tick_index = curr_content_inner_min[1] / tick_res[1];
		int tick_line_index = 0;
		
		for (auto& tick_node : horiz_lines)
			tick_node->setShow(false);

		for (auto& tick_node : horiz_grid_lines)
			tick_node->setShow(false);
		
		for (auto& text_node : horiz_text_node)
			text_node->setShow(false);
		
		while (true)
		{
			float value = tick_index * tick_res[1];
			if (value < curr_content_inner_min[1])
			{
				tick_index++;
				continue;
			}
			
			if (value > curr_content_inner_max[1])
				break;
			
			Vec3 pos(
				box_left_border,
				window.getSize()[1] * (value - curr_content_min[1]) / (curr_content_max[1] - curr_content_min[1]), 
				-1);
			horiz_lines[tick_line_index]->setPos(pos);
			horiz_lines[tick_line_index]->setShow(true);

			horiz_grid_lines[tick_line_index]->setPos(Vec3(
				(curr_content_min[0] + curr_content_max[0])/2.0f,
				value,
				curr_content_min[2] + 0.01));
			horiz_grid_lines[tick_line_index]->setScale(curr_content_max[0] - curr_content_min[0]);
			horiz_grid_lines[tick_line_index]->setShow(true);
			
			char tick_text[128];
			sprintf(tick_text, "%.7g", value);
			horiz_text[tick_line_index]->setText(tick_text);
			
			horiz_text_node[tick_line_index]->setPos(pos-Vec3(tick_size,0,0));
			horiz_text_node[tick_line_index]->setShow(true);
			
			tick_line_index++;
			tick_index++;
		}
	}
}