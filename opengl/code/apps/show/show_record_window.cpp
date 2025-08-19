/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#include "show_record_window.h"

void waitForMessages(NetworkManager* net_manager, std::deque<JSONBMessage>* messages, std::mutex* message_mutex, bool* shutdown)
{
	while (!*shutdown)
	{
		auto in_messages = net_manager->getIncomingJSONBMessagesFromAll();
		
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		
		if (!in_messages.empty())
		{
			std::lock_guard<std::mutex> guard(*message_mutex);
		
			for (const auto& msg : in_messages)
				messages->push_back(msg);
		}
	}
}

void ShowRecordWindow::update(bool debug)
{
	if (shutdown)
		return;
	//if (!window->isShow())
	//	return;

	bool showing_before = window->isShow();

	if (debug)
		v4print "ShowRecordWindow::update - render";
	shutdown = !window->render();

	if (!video_path.empty())
	{
		auto screenshot = window->getScreenImage();
        saveImage(video_path+"/frame_"+std::to_string(video_frame_count)+".png", screenshot);
		video_frame_count++;
	}

	if (shutdown)
	{
		v4print "Closing window:", window_id;
		
		if (net_manager)
		{
			json msg = {
				{"type", "close"},
				{"window", window_id}
			};
			try {
				net_manager->sendJSONBMessage(conn_id, msg, {});
			} catch (...) {}
		}
		
		return;
	}
	else if (showing_before && !window->isShow()) // Window was just hidden
	{
		v4print "Hiding window:", window_id;
		
		if (net_manager)
		{
			json msg = {
				{"type", "hide"},
				{"window", window_id}
			};
			try {
				net_manager->sendJSONBMessage(conn_id, msg, {});
			} catch (...) {}
		}

		return;
	}

	if (debug)
		v4print "ShowRecordWindow::update - camera/controls";
	camera->update(*window.get(), dt, control_info);
	control_info.onUpdate();
}

void ShowRecordWindow::close()
{
	if (shutdown)
		return;
	window->shutdown();
	
	v4print "Closing window:", window_id;
	
	if (net_manager)
	{
		json msg = {
			{"type", "close"},
			{"window", window_id}
		};
		try {
			net_manager->sendJSONBMessage(conn_id, msg, {});
		} catch (...) {}
	}
	
	shutdown = true;
}

void ShowRecordWindow::setup(const json& data, 
	WindowID p_window_id, 
	ConnectionID p_conn_id, 
	NetworkManager* p_net_manager, 
	AssetManager& assets, 
	bool hide_windows)
{
	window_id = p_window_id;
	conn_id = p_conn_id;
	net_manager = p_net_manager;
	
	float width = -1;
	float height = -1;
	float x_position = -1;
	float y_position = -1;
	Vec2 size(-1, -1);
	string title = "show";
	float fps = 30.0f;
	float opacity = 1.0f;
	bool perunit_layout = false;
	
	if (data.contains("window"))
	{
        const auto& window_data = data["window"];

		if (window_data.contains("width"))
        {
            width = window_data["width"];
        }
		if (window_data.contains("height"))
        {
            height = window_data["height"];
        }
		if (window_data.contains("x_position"))
        {
            x_position = window_data["x_position"];
        }
		if (window_data.contains("y_position"))
        {
            y_position = window_data["y_position"];
        }
		if (window_data.contains("perunit_layout"))
        {
            perunit_layout = window_data["perunit_layout"];
        }
		if (window_data.contains("title"))
        {
            title = window_data["title"];
        }
		if (window_data.contains("fps"))
        {
            fps = window_data["fps"];
        }
		if (window_data.contains("opacity"))
        {
            opacity = window_data["opacity"];
        }
	}

	v4print "Window created:", window_id, title;

	if (perunit_layout)
	{
		auto monitor_infos = Window::getMonitorInfo();

		if (monitor_infos.empty())
		{
			width = -1;
			height = -1;
			x_position = -1;
			y_position = -1;
		}
		else
		{
			v4print "Monitors";
			for (const auto& info : monitor_infos)
			{
				v4print "\t", info.pos.transpose(), info.size.transpose();
			}

			v4print "Setting window layout based on monitor dimensions.";

			const auto& curr_monitor = monitor_infos[0];
			if (x_position > 0)
				x_position = curr_monitor.pos[0] + curr_monitor.size[0] * x_position;
			if (y_position > 0)
				y_position = curr_monitor.pos[1] + curr_monitor.size[1] * y_position;
			width = curr_monitor.size[0] * width;
			height = curr_monitor.size[1] * height;

			v4print "\t", "Size:", width, height, "Pos:", x_position, y_position;
		}

		
	}

	WindowOptions opts;
	opts.show = false;
    opts.always_on_top = false;
    window = make_shared<Window>(/* width */ width,
                                 /* height */ height,
                                 /* x_position */ x_position,
                                 /* y_position */ y_position,
                                 /* title */ title,
                                 /* resize_callback */
                                 [&](Window *this_window, const Vec2i &window_size)
                                 {
                                     if (camera)
                                         camera->resizeWindow(*this_window);
                                 },
                                 /* options */ opts);

	window->setHideOnClose(hide_windows);
	window->setOpacity(opacity);

	bound_mgr = std::make_unique<BoundManager>(window.get());
	
	text_tex = loadTexture("/assets/Holstein.png", false, assets);
	
	window->setFrameLock(fps);
	dt = 1.0f / fps;
	
	auto ambient_light = make_shared<AmbientLight>(Vec3(1,1,1)*0.3);
	window->add(ambient_light, Transform3());
	
	Vec3 light_dir(1,0.9,0.8);
	light_dir /= light_dir.norm();
	auto dir_light = make_shared<DirLight>(light_dir, 0.4f);
	window->add(dir_light, Transform3());

	const int on_top_indicator_size = 25;
	MeshData mesh_data(6);
	mesh_data.addQuad2D(Vec2(0,-on_top_indicator_size), Vec2(on_top_indicator_size,0), Vec2(0,0), Vec2(1,1));
	auto mesh = make_shared<Mesh>(mesh_data, Vec3(1,1,0));
	mesh->setCull(false);
	on_top_indicator_node = window->add(mesh, Vec3(0, 0, 0), true, Window::NUM_LAYERS / 2);
	on_top_indicator_node->setParent(window->getWindowTopLeftNode());
	on_top_indicator_node->setShow(false);

	setupCamera(data, assets);
	
	setupInputHandlers(net_manager);
}

void ShowRecordWindow::updateData(const json& data, AssetManager& assets)
{
	if (data.contains("window"))
	{
		if (data["window"].contains("title"))
		{
			string title = data["window"]["title"];
			window->setTitle(title);
		}
		if (data["window"].contains("opacity") && !custom_opacity)
		{
			float opacity = data["window"]["opacity"];
			window->setOpacity(opacity);
		}
	}

	if (data.contains("camera"))
	{
		if (data["camera"].contains("control"))
			camera->update(data["camera"]["control"], *window, assets);
	}
}

void ShowRecordWindow::positionCameraFromObjectBounds()
{
	auto camera_bound_points = bound_mgr->calcCameraBoundPoints();

	if (camera_bound_points.empty())
		return;

	camera->autoFitPoints(*window.get(), camera_bound_points);
}

void ShowRecordWindow::setupCamera(const json& data, AssetManager& assets)
{
	if (data.contains("camera"))
	{
		json cam_data = data["camera"];

		if (cam_data.contains("control"))
		{
			json control_data = cam_data["control"];
							
			if (control_data["type"] == "trackball")
			{
				camera = make_shared<TrackballCamera>();
			}
			else if (control_data["type"] == "spin")
			{
				camera = make_shared<SpinCamera>();
			}
			else if (control_data["type"] == "trajectory")
			{
				camera = make_shared<TrajectoryCamera>();
			}
			else if (control_data["type"] == "static")
			{
				camera = make_shared<StaticCamera>();
			}
			else if (control_data["type"] == "plot")
			{
				camera = make_shared<PlotCamera>();
			}
			else if (control_data["type"] == "plot3d")
			{
				camera = make_shared<Plot3DCamera>();
			}
			camera->text_tex = text_tex;
			camera->setup(control_data, *window.get(), assets);
		}
	}
	else
	{
		throw std::runtime_error("Camera node not present in data.");
	}
	
	if (!camera)
		throw std::runtime_error("No camera defined.");
		
	camera->resizeWindow(*window.get());
}

void ShowRecordWindow::setupInputHandlers(NetworkManager* net_manager)
{
	window->setLeftMouseButtonPressCallback([&, net_manager](){
		control_info.onLeftMouseButtonPress(*window);

		if (net_manager)
		{
			json mouse_click = {
				{"type", "mouse_click"},
				{"window", window_id},
				{"button", "left"},
				{"state", "down"},
				{"pos", toJson(window->getMousePos())},
				{"window_size", toJson(window->getSize())},
			};
			camera->augmentInputEvent(*window, mouse_click);
			net_manager->sendJSONBMessage(conn_id, mouse_click, {});
		}
	});

	window->setLeftMouseButtonReleaseCallback([&, net_manager](){
		control_info.onLeftMouseButtonRelease();

		if (net_manager)
		{
			json mouse_click = {
				{"type", "mouse_click"},
				{"window", window_id},
				{"button", "left"},
				{"state", "up"},
				{"pos", toJson(window->getMousePos())},
				{"window_size", toJson(window->getSize())},
			};
			camera->augmentInputEvent(*window, mouse_click);
			net_manager->sendJSONBMessage(conn_id, mouse_click, {});
		}
	});
	
	window->setRightMouseButtonPressCallback([&, net_manager](){
		control_info.onRightMouseButtonPress(*window);

		if (net_manager)
		{
			json mouse_click = {
				{"type", "mouse_click"},
				{"window", window_id},
				{"button", "right"},
				{"state", "down"},
				{"pos", toJson(window->getMousePos())},
				{"window_size", toJson(window->getSize())},
			};
			camera->augmentInputEvent(*window, mouse_click);
			net_manager->sendJSONBMessage(conn_id, mouse_click, {});
		}
	});

	window->setRightMouseButtonReleaseCallback([&, net_manager](){
		control_info.onRightMouseButtonRelease();

		if (net_manager)
		{
			json mouse_click = {
				{"type", "mouse_click"},
				{"window", window_id},
				{"button", "right"},
				{"state", "up"},
				{"pos", toJson(window->getMousePos())},
				{"window_size", toJson(window->getSize())},
			};
			camera->augmentInputEvent(*window, mouse_click);
			net_manager->sendJSONBMessage(conn_id, mouse_click, {});
		}
	});

	window->setKeyPressCallback([&](int key){
		control_info.onKeyPress(key);

		if (key == GLFW_KEY_V)
		{
			save_flag = true;
		}
		else if (key == GLFW_KEY_T)
		{
			window->toggleAlwaysOnTop();
			on_top_indicator_node->setShow(!on_top_indicator_node->getShow());
		}
		else if (key == GLFW_KEY_O)
		{
			custom_opacity = true;
			float opacity = window->getOpacity();
			if (opacity < 1.0)
				window->setOpacity(1.0);
			else
				window->setOpacity(0.75);
		}
	});
	window->setKeyReleaseCallback([&, net_manager](int key){
		control_info.onKeyRelease(key);

		if (net_manager)
		{
			json key_press = {
				{"type", "key_press"},
				{"window", window_id},
				{"key", key}
			};
			camera->augmentInputEvent(*window, key_press);
			net_manager->sendJSONBMessage(conn_id, key_press, {});
		}		
	});

	window->setScrollCallback([&](float amount){
		control_info.onScroll(amount);
	});
}

shared_ptr<RenderNode> ShowRecordWindow::addFromJSON(const json& obj, const vector<vector<char>>& bufs, AssetManager& assets, JSONBMessage& serialization)
{
	int base_buf_index = 0;
	if (obj.contains("base_buf_index"))
		base_buf_index = obj["base_buf_index"];
	
	if (obj["type"] == "null")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		auto node = window->add(world_from_body);
		return node;
	}
	else if (obj["type"] == "lines")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		VertexDataRecord verts_record, colors_record;
		
		readVertexData(obj["verts"], base_buf_index, bufs, verts_record, &serialization.header["verts"], &serialization.payloads);
		
		if (obj.contains("colors"))
			readVertexData(obj["colors"], base_buf_index, bufs, colors_record, &serialization.header["colors"], &serialization.payloads);


		auto draw_obj = make_shared<Lines>(
			verts_record.map.transpose(),
			colors_record.map.transpose(),
			readDefault(obj, "default_color_rgba", Vec4(1,1,1,1)));


		if (obj.contains("strip"))
			draw_obj->setLineStrip(obj["strip"]);

		if (obj.contains("alpha"))
			draw_obj->setAlpha(true);

		bool overlay = readDefault(obj, "overlay", false);
		auto world_from_body = readTransformSim3(obj["pose"]);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		if (!overlay)
		{
			if(readDefault(obj, "bounds", true))
				bound_mgr->addBoundVerts(verts_record.map.transpose(), node->getWorldTransform());

			if(readDefault(obj, "selectable", false))
			{
				Vec3 centroid = verts_record.map.transpose().colwise().mean();
				MatX centered = verts_record.map.transpose().rowwise() - centroid.transpose();
				VecX norms = centered.rowwise().norm();

				camera->addSelectionWidget(
					world_from_body.translation() + centroid, 
					norms.maxCoeff()*world_from_body.scale,
					node);
			}
		}

		return node;
	}
	else if (obj["type"] == "thick_lines")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		VertexDataRecord verts_record, colors_record;
		
		readVertexData(obj["verts"], base_buf_index, bufs, verts_record, &serialization.header["verts"], &serialization.payloads);
		
		if (obj.contains("colors"))
			readVertexData(obj["colors"], base_buf_index, bufs, colors_record, &serialization.header["colors"], &serialization.payloads);

		auto draw_obj = make_shared<ThickLines>(
			verts_record.map.transpose(),
			colors_record.map.transpose(),
			readDefault(obj, "default_color_rgba", Vec4(1,1,1,1)));


		if (obj.contains("line_width"))
			draw_obj->setLineWidth(obj["line_width"]);

		bool overlay = readDefault(obj, "overlay", false);
		auto world_from_body = readTransformSim3(obj["pose"]);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		if (!overlay)
		{	
			if (readDefault(obj, "bounds", true))
				bound_mgr->addBoundVerts(verts_record.map.transpose(), node->getWorldTransform());

			if(readDefault(obj, "selectable", false))
			{
				Vec3 centroid = verts_record.map.transpose().colwise().mean();
				MatX centered = verts_record.map.transpose().rowwise() - centroid.transpose();
				VecX norms = centered.rowwise().norm();

				camera->addSelectionWidget(
					world_from_body.translation() + centroid, 
					norms.maxCoeff()*world_from_body.scale,
					node);
			}
		}

		return node;
	}
	else if (obj["type"] == "mesh")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		MatX verts = readMatX(obj["verts"]);
		MatX norms = readMatX(obj["norms"]);
		MatX uvs = readMatX(obj["uvs"]);
		
		MeshData data;
		data.verts = verts;
		data.norms = norms;
		data.uvs = uvs;
		
		auto draw_obj = make_shared<Mesh>(
			data,
			readDefault(obj, "color_rgba", Vec4(1,1,1,1)),
			readDefault(obj, "diffuse", 1.0f),
			readDefault(obj, "ambient", 1.0f),
			readDefault(obj, "emissive", 0.0f));
			
		draw_obj->setCull(readDefault(obj, "cull", true));
		draw_obj->setSpecular(readDefault(obj, "specular", 1.0f));
		
		if (obj.contains("tex"))
		{
			shared_ptr<Texture> tex = window->getTextureFromJSON(
				obj["tex"], 
				base_buf_index, 
				bufs, 
				[&assets](const string& path){return assets.getAssetPath(path);},
				&serialization.header["tex"],
				&serialization.payloads);
			draw_obj->setTexture(tex);
		}

		bool overlay = readDefault(obj, "overlay", false);
		auto world_from_body = readTransformSim3(obj["pose"]);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		if (!overlay)
		{
			if (readDefault(obj, "bounds", true))
				bound_mgr->addBoundVerts(verts, node->getWorldTransform());

			if(readDefault(obj, "selectable", false))
			{
				Vec3 centroid = verts.colwise().mean();
				MatX centered = verts.rowwise() - centroid.transpose();
				VecX norms = centered.rowwise().norm();

				camera->addSelectionWidget(
					world_from_body.translation() + centroid, 
					norms.maxCoeff()*world_from_body.scale,
					node);
			}
		}
		return node;
	}
	else if (obj["type"] == "obj_mesh")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		string path = assets.getAssetPath(obj["path"]);
		auto mesh_parts = loadOBJMesh(path);
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		int layer = readDefault(obj, "layer", 0);

		auto node = window->add(world_from_body);
		
		for (auto& part : mesh_parts)
		{
			auto draw_obj = make_shared<Mesh>(
				part.geometry,
				part.diffuse);

			if (!part.diffuse_texture.empty())
			{
				std::string tex_path = fs::path(path).remove_filename().generic_string()+part.diffuse_texture;
				draw_obj->setTexture(window->loadTexture(tex_path, false));
			}
			
			auto sub_node = window->add(draw_obj, TransformSim3(), overlay, layer);
			sub_node->setParent(node);
			if (!overlay)
			{
				if (readDefault(obj, "bounds", true))
					bound_mgr->addBoundVerts(part.geometry.verts, node->getWorldTransform());

				if(readDefault(obj, "selectable", false))
				{
					Vec3 centroid = part.geometry.verts.colwise().mean();
					MatX centered = part.geometry.verts.rowwise() - centroid.transpose();
					VecX norms = centered.rowwise().norm();

					camera->addSelectionWidget(
						world_from_body.translation() + centroid, 
						norms.maxCoeff()*world_from_body.scale,
						sub_node);
				}
			}
		}
		
		return node;
	}
	else if (obj["type"] == "sphere")
	{
		serialization.header = obj;
		serialization.valid = true;

		float rad = readDefault(obj, "rad", 1.0f);
		
		auto draw_obj = make_shared<Sphere>(
			rad, 
			readDefault(obj, "slices", 16), 
			readDefault(obj, "stacks", 16)
		);
		draw_obj->setColor(readDefault(obj, "color_rgb", Vec3(1,1,1)));
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		if (!overlay)
		{
			if (readDefault(obj, "bounds", true))
				bound_mgr->addBoundSphere(rad, node->getWorldTransform());

			if(readDefault(obj, "selectable", false))
			{
				camera->addSelectionWidget(
					world_from_body.translation(), 
					rad*world_from_body.scale,
					node);
			}
		}
		return node;
	}
	else if (obj["type"] == "cylinder")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		float rad = readDefault(obj,"rad",1.0);
		float height = readDefault(obj,"height",1.0);
		auto draw_obj = make_shared<Cylinder>(rad, height, readDefault(obj,"slices",16));
		draw_obj->setColor(readDefault(obj, "color_rgb", Vec3(1,1,1)));
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		if (!overlay)
		{
			if (readDefault(obj, "bounds", true))
				bound_mgr->addBoundSphere(std::max(rad, height/2), node->getWorldTransform());

			if(readDefault(obj, "selectable", false))
			{
				camera->addSelectionWidget(
					world_from_body.translation(), 
					std::max(rad, height/2)*world_from_body.scale,
					node);
			}
		}
		return node;
	}
	else if (obj["type"] == "cone")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		float rad = readDefault(obj,"rad",1.0);
		float height = readDefault(obj,"height",1.0);
		auto draw_obj = make_shared<Cone>(rad, height, readDefault(obj,"slices",16));
		draw_obj->setColor(readDefault(obj, "color_rgb", Vec3(1,1,1)));
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		if (!overlay)
		{
			if (readDefault(obj, "bounds", true))
				bound_mgr->addBoundSphere(std::max(rad, height/2), node->getWorldTransform());

			if(readDefault(obj, "selectable", false))
			{
				camera->addSelectionWidget(
					world_from_body.translation(), 
					std::max(rad, height/2)*world_from_body.scale,
					node);
			}
		}
		return node;
	}
	else if (obj["type"] == "cube")
	{	
		serialization.header = obj;
		serialization.valid = true;

		float rad = readDefault(obj,"rad",1.0);

		auto draw_obj = make_shared<Cube>(rad);
		draw_obj->setColor(readDefault(obj, "color_rgb", Vec3(1,1,1)));
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		if (!overlay)
		{
			if (readDefault(obj, "bounds", true))
				bound_mgr->addBoundSphere(rad, node->getWorldTransform());

			if(readDefault(obj, "selectable", false))
			{
				camera->addSelectionWidget(
					world_from_body.translation(), 
					rad*world_from_body.scale,
					node);
			}
		}
		return node;
	}
	else if (obj["type"] == "plane")
	{	
		serialization.header = obj;
		serialization.valid = true;

		auto rads = readVec2(obj["rads"]);
		auto draw_obj = make_shared<Plane>(rads);
		draw_obj->setColor(readDefault(obj, "color_rgba", Vec4(1,1,1,1)));
		
		if (obj.contains("tex"))
		{
			shared_ptr<Texture> tex = window->getTextureFromJSON(
				obj["tex"], 
				base_buf_index, 
				bufs, 
				[&assets](const string& path){return assets.getAssetPath(path);},
				&serialization.header["tex"],
				&serialization.payloads);
			draw_obj->setTexture(tex);
		}
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		if (!overlay)
		{
			if (readDefault(obj, "bounds", true))
			{
				bound_mgr->addBoundPoint(Vec3(-rads[0],-rads[1],0), node->getWorldTransform());
				bound_mgr->addBoundPoint(Vec3(rads[0],-rads[1],0), node->getWorldTransform());
				bound_mgr->addBoundPoint(Vec3(-rads[0],rads[1],0), node->getWorldTransform());
				bound_mgr->addBoundPoint(Vec3(rads[0],rads[1],0), node->getWorldTransform());
			}

			if(readDefault(obj, "selectable", false))
			{
				camera->addSelectionWidget(
					world_from_body.translation(), 
					rads.maxCoeff()*world_from_body.scale,
					node);
			}
		}
		return node;
	}
	else if (obj["type"] == "arrow")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		auto start = readVec3(obj["start"]);
		auto end = readVec3(obj["end"]);
		float rad = readDefault(obj,"rad",1.0f);
		float dist = (end - start).norm();
		auto draw_obj = make_shared<Arrow>(
			start, 
			end,
			rad,
			readDefault(obj,"slices",16)
		);
		draw_obj->setColor(readDefault(obj, "color_rgba", Vec4{1.0F, 1.0F, 1.0F, 1.0F}));

		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		// TODO: arrow offset?
		if (!overlay)
		{
			if (readDefault(obj, "bounds", true))
				bound_mgr->addBoundSphere(std::max(rad, dist/2), node->getWorldTransform());

			if(readDefault(obj, "selectable", false))
			{
				camera->addSelectionWidget(
					world_from_body.translation(), 
					std::max(rad, dist/2)*world_from_body.scale,
					node);
			}
		}
		return node;
	}
	else if (obj["type"] == "axes")
	{
		serialization.header = obj;
		serialization.valid = true;

		float size = readDefault(obj,"size",1.0f);
		
		auto draw_obj = make_shared<Axes>(size);
		if (obj.contains("x_rgb") && obj.contains("y_rgb") && obj.contains("z_rgb"))
		{
			draw_obj->setColors(
				convert_from<Vec3, json>(obj["x_rgb"]),
				convert_from<Vec3, json>(obj["y_rgb"]),
				convert_from<Vec3, json>(obj["z_rgb"])
			);
		}
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		if (!overlay)
		{
			if (readDefault(obj, "bounds", true))
				bound_mgr->addBoundSphere(size, node->getWorldTransform());

			if(readDefault(obj, "selectable", false))
			{
				camera->addSelectionWidget(
					world_from_body.translation(), 
					size*world_from_body.scale,
					node);
			}
		}
		return node;
	}
	else if (obj["type"] == "circle")
	{
		serialization.header = obj;
		serialization.valid = true;

		float rad = readDefault(obj,"rad",1.0f);
		
		auto draw_obj = make_shared<Circle>(
			rad, 
			readDefault(obj,"thickness",1.0f), 
			readDefault(obj,"slices",16));
		draw_obj->setColor(readDefault(obj, "color_rgb", Vec3(1,1,1)));
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		if (!overlay && readDefault(obj, "bounds", true))
			bound_mgr->addBoundSphere(rad, node->getWorldTransform());
		return node;
	}
	else if (obj["type"] == "grid")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		float rad = readDefault(obj,"rad",10.0f);
		Vec3 normal = readDefault(obj,"normal",Vec3(0,0,1));
		Vec3 right = readDefault(obj,"right",Vec3(1,0,0));
		Vec3 forward = normal.cross(right);
		forward /= forward.norm();
		auto draw_obj = make_shared<Grid>(
			rad,
			normal,
			right,
			readDefault(obj,"cell_size",1.0f),
			readDefault(obj, "color_rgb", Vec3(1,1,1))
		);
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, overlay, readDefault(obj, "layer", 0));
		if (!overlay && readDefault(obj, "bounds", true))
		{
			bound_mgr->addBoundPoint( rad * forward + rad * right, node->getWorldTransform());
			bound_mgr->addBoundPoint( rad * forward - rad * right, node->getWorldTransform());
			bound_mgr->addBoundPoint(-rad * forward + rad * right, node->getWorldTransform());
			bound_mgr->addBoundPoint(-rad * forward - rad * right, node->getWorldTransform());
		}
		return node;
	}
	else if (obj["type"] == "text")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		auto draw_obj = make_shared<Text>(
			obj["text"],
			text_tex,
			readDefault(obj, "color_rgb", Vec3(1,1,1))
		);
		
		Vec2 anchor = readDefault(obj, "anchor", Vec2(0,0));
		draw_obj->setAnchorOffset(anchor);

		draw_obj->setBillboard(readDefault(obj, "billboard", true));
		draw_obj->setYFlip(readDefault(obj, "y_flip", false));
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		return node;
	}
	else if (obj["type"] == "particle")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		const Vec4 default_color = readDefault(obj, "default_color_rgba", Vec4(1,1,1,1));
		const float default_size = readDefault(obj, "size", 0.03f);

        VertexDataRecord verts_record;
		readVertexData(obj["verts"], base_buf_index, bufs, verts_record, &serialization.header["verts"], &serialization.payloads);

        VertexDataRecord colors_record;
		if (obj.contains("colors"))
		{
			readVertexData(obj["colors"], base_buf_index, bufs, colors_record, &serialization.header["colors"], &serialization.payloads);

			if (verts_record.map.cols() != colors_record.map.cols())
			{
				v4print "Verts:", verts_record.map.rows(), verts_record.map.cols();
				v4print "Colors:", colors_record.map.rows(), colors_record.map.cols();
				throw std::runtime_error("Verts and colors must have same size.");
			}
		}
		else
		{
			colors_record.verts = MatX(3,0);
			new (&colors_record.map) MatXMap(colors_record.verts.data(), colors_record.verts.rows(), colors_record.verts.cols());
		}

        VertexDataRecord sizes_record;
        if (obj.contains("sizes"))
        {
            readVertexData(obj["sizes"],
                           base_buf_index,
                           bufs,
                           sizes_record,
                           &serialization.header["sizes"],
                           &serialization.payloads);

            if (verts_record.map.cols() != sizes_record.map.cols())
            {
                v4print "Verts:", verts_record.map.rows(), verts_record.map.cols();
                v4print "Colors:", colors_record.map.rows(), colors_record.map.cols();
                v4print "Sizes:", sizes_record.map.rows(), sizes_record.map.cols();
                throw std::runtime_error("Verts, colors, and sizes must have same size.");
            }
        }
		else
		{
			sizes_record.verts = MatX(1,0);
			new (&sizes_record.map) MatXMap(sizes_record.verts.data(), sizes_record.verts.rows(), sizes_record.verts.cols());
		}

        auto draw_obj = make_shared<InstancedPoints>(
            /* pts */ verts_record.map.transpose(),
            /* colors */ colors_record.map.transpose(),
            /* sizes */ sizes_record.map.transpose(),
            /* default_color */ default_color,
            /* default_size */ default_size);

		if (obj.contains("tex") && !string(obj["tex"]).empty())
		{
			bool filter_nearest = false;
			if (obj.contains("nearest"))
				filter_nearest = obj["nearest"];
			auto tex = loadTexture(obj["tex"], filter_nearest, assets);
			draw_obj->setTexture(tex);
		}

		if (obj.contains("ss_mode"))
			draw_obj->setScreenSpaceMode(obj["ss_mode"]);

		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, overlay, readDefault(obj, "layer", 0));
		if (!overlay && readDefault(obj, "bounds", true))
			bound_mgr->addBoundVerts(verts_record.map.transpose(), node->getWorldTransform());
		return node;
	}
	else if (obj["type"] == "sprite")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		float diffuse = readDefault(obj, "diffuse", 1.0f);
		float ambient = readDefault(obj, "ambient", 1.0f);
		Vec3 color = readDefault(obj, "color_rgb", Vec3(1,1,1));

		if (!obj.contains("tex"))
		{
			throw std::runtime_error("No texture data for sprite.");
		}

		shared_ptr<Texture> tex = window->getTextureFromJSON(
			obj["tex"], 
			base_buf_index, 
			bufs, 
			[&assets](const string& path){return assets.getAssetPath(path);},
			&serialization.header["tex"],
			&serialization.payloads
		);
		
		if (!tex)
		{
			throw std::runtime_error("No texture for sprite.");
		}
		
		bool x_flip = false;
		if (obj.contains("x_flip"))
			x_flip = obj["x_flip"];
		
		bool y_flip = false;
		if (obj.contains("y_flip"))
			y_flip = obj["y_flip"];

		auto draw_obj = make_shared<Sprite>(tex, Vec2i::Zero(), x_flip, y_flip);
		draw_obj->setDiffuse(color * diffuse);
		draw_obj->setAmbient(color * ambient);

		if (obj.contains("normal_tex"))
		{
			shared_ptr<Texture> tex = window->getTextureFromJSON(
				obj["normal_tex"], 
				base_buf_index, 
				bufs, 
				[&assets](const string& path){return assets.getAssetPath(path);},
				&serialization.header["normal_tex"],
				&serialization.payloads
			);
			draw_obj->setNormalSpriteSheet(tex);
		}

		auto world_from_body = readTransformSim3(obj["pose"]);

		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, overlay, readDefault(obj, "layer", 0));
		
		float aspect = (float)tex->size()[0] / tex->size()[1];
		
		if (!overlay && readDefault(obj, "bounds", true))
		{
			bound_mgr->addBoundPoint(Vec3(-0.5*aspect,-0.5,0), node->getWorldTransform());
			bound_mgr->addBoundPoint(Vec3(-0.5*aspect, 0.5,0), node->getWorldTransform());
			bound_mgr->addBoundPoint(Vec3( 0.5*aspect, 0.5,0), node->getWorldTransform());
			bound_mgr->addBoundPoint(Vec3( 0.5*aspect,-0.5,0), node->getWorldTransform());
		}
		
		return node;
	}
	else if (obj["type"] == "dir_light")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		auto draw_obj = make_shared<DirLight>(readVec3(obj["dir"]), obj["strength"]);
		
		auto world_from_body = TransformSim3();
		auto node = window->add(draw_obj, world_from_body, false, -100);
		return node;
	}
	else if (obj["type"] == "ambient_light")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		auto draw_obj = make_shared<AmbientLight>(readVec3(obj["strength"]));
		
		auto world_from_body = TransformSim3();
		auto node = window->add(draw_obj, world_from_body, false, -100);
		return node;
	}
	
	throw std::runtime_error(string("Invalid object type: ") + string(obj["type"]));
	return NULL;
}

json ShowRecordWindow::produceSceneJSON(vector<vector<char>>* bufs)
{
	json scene = window->produceSceneJSON(bufs);
	
	if (window_id > 0)
		scene["window_id"] = window_id;
	
	scene["camera"]["control"] = camera->serialize();

	return scene;
}

shared_ptr<Texture> ShowRecordWindow::loadTexture(const string& path, bool nearest, AssetManager& assets)
{
	string full_path = assets.getAssetPath(path);

	if (find(texture_bank, full_path))
	{
		// TODO: we don't allow one texture with two filter modes - problem?
		return texture_bank[full_path];
	}
	
	auto tex = window->loadTexture(full_path, nearest);
	texture_bank[full_path] = tex;
	return tex;
}