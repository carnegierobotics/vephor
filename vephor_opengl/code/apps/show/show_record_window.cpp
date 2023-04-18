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

void ShowRecordWindow::update()
{
	if (shutdown)
		return;
	shutdown = !window->render();
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
	camera->update(*window.get(), dt, control_info);
	control_info.total_scroll_amount = 0.0f;
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

void ShowRecordWindow::setup(const json& data, WindowID p_window_id, ConnectionID p_conn_id, NetworkManager* p_net_manager, AssetManager& assets)
{
	window_id = p_window_id;
	conn_id = p_conn_id;
	net_manager = p_net_manager;
	
	int width = -1;
	int height = -1;
	Vec2 size(-1, -1);
	string title = "show";
	
	if (data.contains("window"))
	{
		width = data["window"]["width"];
		height = data["window"]["height"];
		title = data["window"]["title"];
	}
	
	v4print "Window created:", window_id, title;
	
	WindowOptions opts;
	opts.show = false;
	window = make_shared<Window>(width, height, title, [&](Window* this_window, const Vec2i& window_size){
		if (camera)
			camera->resizeWindow(*this_window);
	}, opts);

	window->setHideOnClose(true);
	
	text_tex = loadTexture("/assets/Holstein.png", false, assets);
	
	const float fps = 60.0f;
	window->setFrameLock(fps);
	dt = 1.0f / fps;
	
	auto ambient_light = make_shared<AmbientLight>(Vec3(1,1,1)*0.3);
	window->add(ambient_light, Transform3());
	
	Vec3 light_dir(1,0.9,0.8);
	light_dir /= light_dir.norm();
	auto dir_light = make_shared<DirLight>(light_dir, 0.4f);
	window->add(dir_light, Transform3());

	setupCamera(data, assets);
	
	setupInputHandlers(net_manager);
}

void ShowRecordWindow::addBoundPoint(const Vec3& pt, const TransformSim3& world_from_body)
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

void ShowRecordWindow::addBoundVerts(const MatXRef& verts, const TransformSim3& world_from_body)
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

void ShowRecordWindow::addBoundSphere(float rad, const TransformSim3& world_from_body)
{
	addBoundPoint(Vec3(-rad,0,0), world_from_body);
	addBoundPoint(Vec3(rad,0,0), world_from_body);
	addBoundPoint(Vec3(0,-rad,0), world_from_body);
	addBoundPoint(Vec3(0,rad,0), world_from_body);
	addBoundPoint(Vec3(0,0,-rad), world_from_body);
	addBoundPoint(Vec3(0,0,rad), world_from_body);
}

void ShowRecordWindow::positionCameraFromObjectBounds()
{
	if (object_bound_in_world_min[0] > object_bound_in_world_max[0])
		return;
	
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
			else if (control_data["type"] == "static")
			{
				camera = make_shared<StaticCamera>();
			}
			else if (control_data["type"] == "plot")
			{
				camera = make_shared<PlotCamera>();
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
	window->setLeftMouseButtonPressCallback([&](){
		Vec2 pos = window->getMousePos();

		control_info.drag_start_mouse_pos = pos;
		control_info.drag_cam_from_world = window->getCamFromWorld();

		control_info.left_drag_on = true;
	});

	window->setLeftMouseButtonReleaseCallback([&](){
		control_info.left_drag_on = false;
	});
	
	window->setRightMouseButtonPressCallback([&](){
		Vec2 pos = window->getMousePos();

		control_info.drag_start_mouse_pos = pos;
		control_info.drag_cam_from_world = window->getCamFromWorld();

		control_info.right_drag_on = true;
	});

	window->setRightMouseButtonReleaseCallback([&](){
		control_info.right_drag_on = false;
	});

	window->setKeyPressCallback([&](int key){
		control_info.key_down[key] = true;
		refreshKeyMotion();

		if (key == GLFW_KEY_V)
		{
			save_flag = true;
		}
	});
	window->setKeyReleaseCallback([&, net_manager](int key){
		control_info.key_down[key] = false;
		refreshKeyMotion();
		if (net_manager)
		{
			json key_press = {
				{"type", "key_press"},
				{"window", window_id},
				{"key", key}
			};
			net_manager->sendJSONBMessage(conn_id, key_press, {});
		}		
	});

	window->setScrollCallback([&](float amount){
		control_info.total_scroll_amount += amount;
	});
}

void ShowRecordWindow::refreshKeyMotion()
{
	control_info.key_motion = Vec3::Zero();
	if (control_info.key_down[GLFW_KEY_D])
	{
		control_info.key_motion[0] = 1;
	}
	if (control_info.key_down[GLFW_KEY_A])
	{
		control_info.key_motion[0] = -1;
	}
	if (control_info.key_down[GLFW_KEY_W])
	{
		control_info.key_motion[1] = 1;
	}
	if (control_info.key_down[GLFW_KEY_S])
	{
		control_info.key_motion[1] = -1;
	}
	if (control_info.key_down[GLFW_KEY_F])
	{
		control_info.key_motion[2] = 1;
	}
	if (control_info.key_down[GLFW_KEY_R])
	{
		control_info.key_motion[2] = -1;
	}
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

		bool overlay = readDefault(obj, "overlay", false);
		auto world_from_body = readTransformSim3(obj["pose"]);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		if (!overlay)
			addBoundVerts(verts_record.map.transpose(), node->getWorldTransform());

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
			addBoundVerts(verts_record.map.transpose(), node->getWorldTransform());

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
			readDefault(obj, "color_rgba", Vec4(1,1,1,1)));
			
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
			addBoundVerts(verts, node->getWorldTransform());
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
		
		for (const auto& part : mesh_parts)
		{
			auto draw_obj = make_shared<Mesh>(
				part.geometry,
				part.diffuse);
			
			auto sub_node = window->add(draw_obj, TransformSim3(), overlay, layer);
			sub_node->setParent(node);
			if (!overlay)
				addBoundVerts(part.geometry.verts, node->getWorldTransform());
		}
		
		return node;
	}
	else if (obj["type"] == "sphere")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		auto draw_obj = make_shared<Sphere>(obj["rad"], obj["slices"], obj["stacks"]);
		draw_obj->setColor(readDefault(obj, "color_rgb", Vec3(1,1,1)));
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		if (!overlay)
			addBoundSphere(obj["rad"], node->getWorldTransform());
		return node;
	}
	else if (obj["type"] == "cylinder")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		float rad = obj["rad"];
		float height = obj["height"];
		auto draw_obj = make_shared<Cylinder>(rad, height, obj["slices"]);
		draw_obj->setColor(readDefault(obj, "color_rgb", Vec3(1,1,1)));
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		if (!overlay)
			addBoundSphere(std::max(rad, height/2), node->getWorldTransform());
		return node;
	}
	else if (obj["type"] == "cone")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		float rad = obj["rad"];
		float height = obj["height"];
		auto draw_obj = make_shared<Cone>(rad, height, obj["slices"]);
		draw_obj->setColor(readDefault(obj, "color_rgb", Vec3(1,1,1)));
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		if (!overlay)
			addBoundSphere(std::max(rad, height/2), node->getWorldTransform());
		return node;
	}
	else if (obj["type"] == "cube")
	{	
		serialization.header = obj;
		serialization.valid = true;

		auto draw_obj = make_shared<Cube>(obj["rad"]);
		draw_obj->setColor(readDefault(obj, "color_rgb", Vec3(1,1,1)));
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		if (!overlay)
			addBoundSphere(obj["rad"], node->getWorldTransform());
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
			addBoundPoint(Vec3(-rads[0],-rads[1],0), node->getWorldTransform());
			addBoundPoint(Vec3(rads[0],-rads[1],0), node->getWorldTransform());
			addBoundPoint(Vec3(-rads[0],rads[1],0), node->getWorldTransform());
			addBoundPoint(Vec3(rads[0],rads[1],0), node->getWorldTransform());
		}
		return node;
	}
	else if (obj["type"] == "arrow")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		auto start = readVec3(obj["start"]);
		auto end = readVec3(obj["end"]);
		float rad = obj["rad"];
		float dist = (end - start).norm();
		auto draw_obj = make_shared<Arrow>(
			start, 
			end,
			rad,
			obj["slices"]
		);
		draw_obj->setColor(readDefault(obj, "color_rgb", Vec3(1,1,1)));
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		// TODO: arrow offset?
		if (!overlay)
			addBoundSphere(std::max(rad, dist/2), node->getWorldTransform());
		return node;
	}
	else if (obj["type"] == "axes")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		auto draw_obj = make_shared<Axes>(obj["size"]);
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		if (!overlay)
			addBoundSphere(obj["size"], node->getWorldTransform());
		return node;
	}
	else if (obj["type"] == "circle")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		auto draw_obj = make_shared<Circle>(obj["rad"], obj["thickness"], obj["slices"]);
		draw_obj->setColor(readDefault(obj, "color_rgb", Vec3(1,1,1)));
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		if (!overlay)
			addBoundSphere(obj["rad"], node->getWorldTransform());
		return node;
	}
	else if (obj["type"] == "grid")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		float rad = obj["rad"];
		Vec3 normal = readVec3(obj["normal"]);
		Vec3 right = readVec3(obj["right"]);
		Vec3 forward = normal.cross(right);
		forward /= forward.norm();
		auto draw_obj = make_shared<Grid>(
			rad,
			normal,
			right,
			obj["cell_size"],
			readDefault(obj, "color_rgb", Vec3(1,1,1))
		);
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, overlay, readDefault(obj, "layer", 0));
		if (!overlay)
		{
			addBoundPoint( rad * forward + rad * right, node->getWorldTransform());
			addBoundPoint( rad * forward - rad * right, node->getWorldTransform());
			addBoundPoint(-rad * forward + rad * right, node->getWorldTransform());
			addBoundPoint(-rad * forward - rad * right, node->getWorldTransform());
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
		
		auto world_from_body = readTransformSim3(obj["pose"]);
		auto node = window->add(draw_obj, world_from_body, readDefault(obj, "overlay", false), readDefault(obj, "layer", 0));
		return node;
	}
	else if (obj["type"] == "particle")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		Vec4 default_color = readDefault(obj, "default_color_rgb", Vec4(1,1,1,1));

		VertexDataRecord verts_record, colors_record;

		readVertexData(obj["verts"], base_buf_index, bufs, verts_record, &serialization.header["verts"], &serialization.payloads);
		
		if (obj.contains("colors"))
			readVertexData(obj["colors"], base_buf_index, bufs, colors_record, &serialization.header["colors"], &serialization.payloads);

		auto draw_obj = make_shared<InstancedPoints>(
			verts_record.map.transpose(),
			colors_record.map.transpose(),
			readDefault(obj, "default_color_rgb", Vec4(1,1,1,1)));
		
		if (obj.contains("tex") && !string(obj["tex"]).empty())
		{
			bool filter_nearest = false;
			if (obj.contains("tex_filter_nearest"))
				filter_nearest = obj["tex_filter_nearest"];
			auto tex = loadTexture(obj["tex"], filter_nearest, assets);
			draw_obj->setTexture(tex);
		}
		
		if (obj.contains("size"))
			draw_obj->setSize(obj["size"]);
		
		if (obj.contains("ss_mode"))
			draw_obj->setScreenSpaceMode(obj["ss_mode"]);

		auto world_from_body = readTransformSim3(obj["pose"]);
		bool overlay = readDefault(obj, "overlay", false);
		auto node = window->add(draw_obj, world_from_body, overlay, readDefault(obj, "layer", 0));
		if (!overlay)
			addBoundVerts(verts_record.map.transpose(), node->getWorldTransform());
		return node;
	}
	else if (obj["type"] == "sprite")
	{
		serialization.header = obj;
		serialization.valid = true;
		
		float diffuse = readDefault(obj, "diffuse", 1.0f);
		float ambient = readDefault(obj, "ambient", 1.0f);
		Vec3 color = readDefault(obj, "color_rgb", Vec3(1,1,1));

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
		
		if (!overlay)
		{
			addBoundPoint(Vec3(-0.5*aspect,-0.5,0), node->getWorldTransform());
			addBoundPoint(Vec3(-0.5*aspect, 0.5,0), node->getWorldTransform());
			addBoundPoint(Vec3( 0.5*aspect, 0.5,0), node->getWorldTransform());
			addBoundPoint(Vec3( 0.5*aspect,-0.5,0), node->getWorldTransform());
		}
		
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