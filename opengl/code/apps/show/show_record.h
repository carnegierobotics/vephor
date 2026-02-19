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

#include "show_record_window.h"

struct FlagRecord
{
	string name;
	bool toggle;
	bool state = false;
	shared_ptr<RenderNode> on_back;
	shared_ptr<RenderNode> off_back;
};

struct FlagsRecord
{
	vector<FlagRecord> flags;
	std::chrono::time_point<std::chrono::steady_clock> last_update;
};

struct ShowRecord
{
	// Window management
	unordered_map<WindowID, shared_ptr<ShowRecordWindow>> windows;
	unordered_set<WindowID> closed_windows;
	bool shutdown = false;
	bool wait_flag = false;
	unordered_map<ConnectionID, shared_ptr<Window>> control_window_per_conn;
	shared_ptr<Texture> text_tex;
	int exit_counter = 0;
	bool keep_windows_hidden = false;
	
	// Object management
	unordered_map<ObjectID, shared_ptr<RenderNode>> objects_by_id;
	AssetManager assets;
	
	// Message handling
	bool network_mode = false;
	NetworkManager net_manager;
	std::deque<JSONBMessage> message_list;
	std::mutex message_mutex;
	std::thread message_thread;
	bool had_connection = false;
	bool client_mode = false;
	string host;
	int port;
	string record_path;
	string video_path;
	int message_index = 0;
	std::chrono::steady_clock::time_point start_time;
	std::chrono::time_point<std::chrono::high_resolution_clock> path_start_time;
	float first_message_time = 0;
	float playback_speed = 1;
	string input_path;
	unordered_map<ConnectionID, FlagsRecord> flags_per_conn;

	ShowRecord()
	{
		assets.addFolder(getBaseAssetDir());
		start_time = std::chrono::steady_clock::now();
	}

    void setParent(const shared_ptr<RenderNode> &node,
                   WindowID window_id,
                   const string &parent_node,
                   ConnectionID conn_id)
    {
        if (windows[window_id].get() == nullptr)
        {
            throw std::runtime_error("Null window found in setParent.");
        }

        if (parent_node == "window_top_left")
        {
            node->setParent(windows[window_id]->window->getWindowTopLeftNode());
        }
        else if (parent_node == "window_top")
        {
            node->setParent(windows[window_id]->window->getWindowTopNode());
        }
        else if (parent_node == "window_top_right")
        {
            node->setParent(windows[window_id]->window->getWindowTopRightNode());
        }
        else if (parent_node == "window_left")
        {
            node->setParent(windows[window_id]->window->getWindowLeftNode());
        }
        else if (parent_node == "window_center")
        {
            node->setParent(windows[window_id]->window->getWindowCenterNode());
        }
        else if (parent_node == "window_right")
        {
            node->setParent(windows[window_id]->window->getWindowRightNode());
        }
        else if (parent_node == "window_bottom_left")
        {
            node->setParent(windows[window_id]->window->getWindowBottomLeftNode());
        }
        else if (parent_node == "window_bottom")
        {
            node->setParent(windows[window_id]->window->getWindowBottomNode());
        }
        else if (parent_node == "window_bottom_right")
        {
            node->setParent(windows[window_id]->window->getWindowBottomRightNode());
        }
        else if (!parent_node.empty())
        {
            node->setParent(objects_by_id[static_cast<int64_t>(std::stoi(parent_node)) | (conn_id << 32)]);
        }
    }

	void setupWindow(const json& data, ConnectionID conn_id, WindowID& window_id, bool& window_added, bool& window_closed)
	{
		window_added = false;
		window_closed = false;
		
		WindowID base_window_id = 0;
		if (data.contains("window_id"))
			base_window_id = (WindowID)data["window_id"];
		
		window_id = base_window_id | (conn_id << 32);
		
		if (find(closed_windows, window_id))
		{
			window_closed = true;
			return;
		}
		
		if (find(windows, window_id))
		{
			if (!keep_windows_hidden)
				windows[window_id]->window->show();
			windows[window_id]->updateData(data, assets);
			return;
		}
		
		window_added = true;
		
		windows[window_id] = make_shared<ShowRecordWindow>();
		
		NetworkManager* net_manager_ptr = NULL;
		if (network_mode)
			net_manager_ptr = &net_manager;
		windows[window_id]->setup(data, base_window_id, conn_id, net_manager_ptr, assets, network_mode);

		if (!video_path.empty())
		{
			string window_video_path = video_path+"/window_"+std::to_string(window_id);
			v4print "Writing frames to", window_video_path;
			fs::create_directories(window_video_path);
			windows[window_id]->video_path = window_video_path;
		}
	}
	void connectClient(const string& p_host, int p_port)
	{
		client_mode = true;

		host = p_host;
		port = p_port;
		
		net_manager.connectClient(true, host, port);
	
		v4print "Connected.";
		
		startNetwork();
	}
	void connectServer(int port)
	{
		net_manager.connectServer(false, port);
		
        v4print "Connected.";
		
		startNetwork();
	}
	void startNetwork()
	{
		if (!network_mode)
		{
        	assets.addFolder(fs::current_path().string());
			message_thread = std::thread(waitForMessages, &net_manager, &message_list, &message_mutex, &shutdown);
			network_mode = true;
		}
	}
	void setupFromPath(const string& path)
	{
		input_path = path;
		
		assets.addFolder(path);

		vector<int> msg_ids;
		if (fs::exists(path+"/meta.json"))
		{
			std::ifstream meta_fin(path+"/meta.json");
			json meta_data = json::parse(meta_fin);
			auto msg_ids_json = meta_data["messages"];
			
			for (json msg_id : msg_ids_json)
			{
				msg_ids.push_back(msg_id);
			}
		}
		else
		{
			for (const auto & entry : fs::directory_iterator(path))
			{
				if (fs::is_directory(entry.path()))
				{
					string entry_str = entry.path().string();
					try {
						msg_ids.push_back(std::stoi(entry.path().filename()));
					} catch (...) {
						continue;
					}
				}
			}
			std::sort(msg_ids.begin(), msg_ids.end());
		}

		if (msg_ids.empty())
		{
			v4print "Selected directory does not appear to contain a vephor scene.  Exiting.";
			std::exit(1);
		}

        for (json msg_id : msg_ids)
		{
			string msg_dir = path + "/" + std::to_string((int)msg_id);
			
			std::ifstream msg_fin(msg_dir+"/msg.json");
			json msg_data = json::parse(msg_fin);
			msg_data["conn_id"] = 0;

			JSONBMessage msg;
			msg.valid = true;
			msg.header = msg_data;
			
			// Load buffers on disk into buffer slots
			size_t i = 0;
			while (true)
			{
				auto buf_file = msg_dir+"/msg_buf_"+to_string(i);
				if (!fs::exists(buf_file))
				{
					break;
				}
			
				ifstream fin(buf_file, std::ios::binary | std::ios::ate);
				std::streamsize size = fin.tellg();
				fin.seekg(0, std::ios::beg);
				
				vector<char> buf(size);
				fin.read(buf.data(), size);
				msg.payloads.push_back(std::move(buf));
				
				i++;
			}
			
			message_list.push_back(msg);
		}

		path_start_time = std::chrono::high_resolution_clock::now();
		if (!message_list.empty() && message_list[0].header.contains("time"))
		{
			first_message_time = (float)message_list[0].header["time"];
			v4print "First message time:", first_message_time;
		}
	}
	void handleMetadataMessage(const JSONBMessage& jsonb_message, ConnectionID conn_id)
	{
		const auto& message = jsonb_message.header;

		if (!message["flags"].empty() && !find(control_window_per_conn,conn_id))
		{
			v4print "Setting up viz control window.";

			const float flag_scale = 1.25f;
			const int flag_offset = 32.5 * flag_scale;
			const int flag_size = 20 * flag_scale;
			const int flag_width = 200 * flag_scale;
			const int flag_gap = 12.5 * flag_scale;
			const int flags_per_col = 15;

			int flag_window_width = (flag_width + flag_gap) * ceil((float)message["flags"].size()/flags_per_col) + flag_gap;
			int flag_window_height = flag_offset*std::min((int)message["flags"].size(),flags_per_col) + flag_gap;

			string name = "Viz Control";
			string inner_name;
			if (message.contains("name"))
					inner_name = message["name"];
			if (!inner_name.empty())
				name = inner_name + " " + name;
			WindowOptions control_window_opts;
			control_window_opts.always_on_top = true;
			auto control_window = make_shared<Window>(
				flag_window_width,
				flag_window_height,
				name,
				(WindowResizeCallback)NULL,
				control_window_opts);
			control_window->setIgnoreClose(true);
			
			text_tex = control_window->loadTexture(assets.getAssetPath("/assets/Holstein.png"));
			
			const float fps = 60.0f;
			control_window->setFrameLock(fps);
			
			FlagsRecord flags_record;

			vector<json> message_flags;
			for (const auto& flag : message["flags"])
			{
				message_flags.push_back(flag);
			}
			std::sort(message_flags.begin(), message_flags.end(), [](const auto& l, const auto& r){
				return l["name"] < r["name"];
			});
			
			for (const auto& flag : message_flags)
			{
				float hpos = (int)(flags_record.flags.size()/flags_per_col)*(flag_width+flag_gap)+flag_gap;
				float vpos = -((int)(flags_record.flags.size()%flags_per_col)+1)*flag_offset;

				bool toggle = flag["toggle"];
				
				shared_ptr<RenderNode> on_back;
				shared_ptr<RenderNode> off_back;

				if (toggle)
				{
					{
						MeshData mesh_data(6);
						mesh_data.addQuad2D(Vec2(0,0), Vec2(flag_width,flag_size), Vec2(0,0), Vec2(1,1));
						auto mesh = make_shared<Mesh>(mesh_data, Vec3(0.2,0.5,0.2));
						mesh->setCull(false);
						on_back = control_window->add(mesh, Vec3(hpos, vpos, 0), true);
						on_back->setParent(control_window->getWindowTopLeftNode());
						on_back->setShow(false);
					}

					{
						MeshData mesh_data(6);
						mesh_data.addQuad2D(Vec2(0,0), Vec2(flag_width,flag_size), Vec2(0,0), Vec2(1,1));
						auto mesh = make_shared<Mesh>(mesh_data, Vec3(0.5,0.2,0.2));
						mesh->setCull(false);
						off_back = control_window->add(mesh, Vec3(hpos, vpos, 0), true);
						off_back->setParent(control_window->getWindowTopLeftNode());
					}
				}
				else
				{
					MeshData mesh_data(6);
					mesh_data.addQuad2D(Vec2(0,0), Vec2(flag_width,flag_size), Vec2(0,0), Vec2(1,1));
					auto mesh = make_shared<Mesh>(mesh_data, Vec3(0.5,0.5,0.5));
					mesh->setCull(false);
					on_back = control_window->add(mesh, Vec3(hpos, vpos, 0), true);
					on_back->setParent(control_window->getWindowTopLeftNode());
				}

				
				auto text = make_shared<Text>(flag["name"], text_tex);
				text->setColor(Vec3(1,1,1));
				auto text_node = control_window->add(text, Vec3(hpos, vpos, 0), true, 1);
				Vec2 text_size = text->getSize();
				float scale_from_x = flag_size;
				if (text_size[0] > 0)
					scale_from_x = flag_width * 0.95f / text_size[0];
				text_node->setScale(std::min((float)flag_size, scale_from_x));
				text_node->setParent(control_window->getWindowTopLeftNode());
				
				flags_record.flags.push_back({flag["name"], toggle, flag["state"], on_back, off_back});
			}
			flags_per_conn[conn_id] = flags_record;
			
			auto control_window_ptr = control_window.get();
			
			control_window->setLeftMouseButtonReleaseCallback([this, control_window_ptr, conn_id, flag_gap, flag_width, flag_offset, flag_size](){
				v4print "(Control window) Left click @", control_window_ptr->getMousePos().transpose();
				
				auto& flags_record = flags_per_conn[conn_id];
				
				for (int i = 0; i < flags_record.flags.size(); i++)
				{
					Vec2 corner_rel_pos = control_window_ptr->getMousePos() - Vec2(0, control_window_ptr->getSize()[1]);
					float hpos = (int)(i/flags_per_col)*(flag_width+flag_gap)+flag_gap;
					float vpos = -(i%flags_per_col+1)*flag_offset;
					corner_rel_pos[0] -= hpos;
					corner_rel_pos[1] -= vpos;
					
					if (corner_rel_pos[0] >= 0 && corner_rel_pos[1] >= 0 &&
						corner_rel_pos[0] <= flag_width && corner_rel_pos[1] <= flag_size)
					{
						v4print "Clicked", flags_record.flags[i].name;
						if (flags_record.flags[i].toggle)
						{
							flags_record.flags[i].state = !flags_record.flags[i].state;

							if (flags_record.flags[i].state)
							{
								flags_record.flags[i].on_back->setShow(true);
								flags_record.flags[i].off_back->setShow(false);
							}
							else
							{
								flags_record.flags[i].on_back->setShow(false);
								flags_record.flags[i].off_back->setShow(true);
							}
						}
						else
							flags_record.flags[i].state = true;
					}
				}
			});

			control_window_per_conn[conn_id] = control_window;
			
			flags_per_conn[conn_id].last_update = std::chrono::steady_clock::now();
		}
	}
	bool handleSceneMessage(const JSONBMessage& jsonb_message, ConnectionID conn_id, bool verbose = false)
	{
		const auto& message = jsonb_message.header;

		json data = message["data"];
				
		WindowID window_id;
		bool window_added, window_closed;
		setupWindow(data, conn_id, window_id, window_added, window_closed);
		
		if (window_closed)
		{
			if (network_mode)
			{
				json msg = {
					{"type", "close"},
					{"window", window_id}
				};	
				try {
					net_manager.sendJSONBMessage(conn_id, msg, {});
				} catch (...) {}
			}

			message_list.pop_front();
			return false;
		}
	
		for (const auto& obj : data["objects"])
		{
			ObjectID id = obj["id"];
			id |= ((ObjectID)conn_id << 32);
			
			if (find(objects_by_id, id))
			{
				auto& render_obj = objects_by_id[id];
				
				if (obj.contains("pose"))
					render_obj->setTransform(readTransform3(obj["pose"]));
				
				if (obj.contains("scale"))
					render_obj->setScale(obj["scale"]);
			
				if (obj.contains("show"))
					render_obj->setShow(obj["show"]);
			
				if (obj.contains("destroy") && obj["destroy"])
				{
					if (verbose)
						v4print "Destroying object:", id;
					render_obj->setDestroy();
					objects_by_id.erase(id);
				}
				
				continue;
			}
			
			if (verbose)
				v4print "Object:", id, obj["type"];

			// Object starts destroyed, don't bother
			if (obj.contains("destroy") && obj["destroy"])
				continue;
			
			JSONBMessage serialization;
			auto node = windows[window_id]->addFromJSON(obj, jsonb_message.payloads, assets, serialization);
			node->setName(std::to_string(id));
			objects_by_id[id] = node;

			if (obj.contains("show"))
				node->setShow(obj["show"]);
			
			node->setObjectSerialization(serialization);
		}
		
		for (const auto& obj : data["objects"])
		{
			ObjectID id = obj["id"];
			id |= ((ObjectID)conn_id << 32);
			
			if (find(objects_by_id, id))
			{
				auto& render_obj = objects_by_id[id];
				
				string parent;
				if (obj.contains("pose_parent"))
				{
					parent = obj["pose_parent"];
				}
				setParent(render_obj, window_id, parent, conn_id);
			}
		}
		
		if (window_added)
		{
			windows[window_id]->positionCameraFromObjectBounds();
			if (!keep_windows_hidden)
				windows[window_id]->window->show();
		}

		return true;
	}
	void handleIncomingMessages(bool verbose = false)
	{
		// Check to see if additional messages have arrived
		while (!message_list.empty())
		{
			std::lock_guard<std::mutex> guard(message_mutex);

			const auto& jsonb_message = *message_list.begin();
			const auto& message = jsonb_message.header;

			ConnectionID conn_id = message["conn_id"];
			float conn_time;
			if (conn_id == 0)
			{
				auto end_time = std::chrono::high_resolution_clock::now();
				conn_time = std::chrono::duration<float, std::milli>(end_time-path_start_time).count() / 1000.0f;
			}
			else
			{
				try {
					conn_time = net_manager.getConnTime(conn_id);
				}
				catch (...) {
					message_list.pop_front();
					continue;
				}
			}
			if (message.contains("time"))
			{
				if ((float)message["time"] - first_message_time > conn_time * playback_speed)
					break;
			}
			
			if (message["type"] == "metadata")
			{
				handleMetadataMessage(jsonb_message, conn_id);
			}
			else if (message["type"] == "file")
			{
				string name = message["name"];

				auto file_path = fs::path(name);
				auto file_folder = file_path.parent_path();

				if (!fs::exists(file_folder))
					fs::create_directory(file_folder);

				v4print "File name:", name;

				const auto& file_buf = jsonb_message.payloads[0];
				v4print "File size:", file_buf.size();

				ofstream fout(file_path, std::ios::binary | std::ios::out);
				fout.write(file_buf.data(), file_buf.size());
				fout.close();
				
				if (!record_path.empty())
				{
					if (!fs::exists(record_path+"/scene_assets"))
					{
						fs::create_directory(record_path+"/scene_assets");
					}
					ofstream fout(record_path+"/"+file_path.string(), std::ios::binary | std::ios::out);
					fout.write(file_buf.data(), file_buf.size());
					fout.close();
				}
			}
			else if (message["type"] == "heartbeat")
			{
				WindowID base_window_id = 0;
				if (message.contains("window_id"))
					base_window_id = (WindowID)message["window_id"];
				
				WindowID window_id = base_window_id | ((WindowID)conn_id << 32);
				
				if (find(closed_windows, window_id))
				{
					if (network_mode)
					{
						json msg = {
							{"type", "close"},
							{"window", window_id}
						};	
						try {
							net_manager.sendJSONBMessage(conn_id, msg, {});
						} catch (...) {}
					}
				}
			}
			else if (message["type"] == "scene")
			{
				if(!handleSceneMessage(jsonb_message, conn_id, verbose))
					continue;
			}
			else if (message["type"] == "wait")
			{
				wait_flag = true;
				v4print "Wait flag set.";
			}
			
			if (!record_path.empty())
			{
				json message_copy = message;
				message_copy["conn_id"] = 0;
				if (!message_copy.contains("time"))
				{
					auto end_time = std::chrono::steady_clock::now();
					message_copy["time"] = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() / 1000.0f;
				}
				string msg_dir = record_path + "/" + std::to_string(message_index);
				writeJSONBMessageToFile(message_copy, jsonb_message.payloads, msg_dir);
			}
			
			message_index++;
			
			message_list.pop_front();
		}
	}
	void handleOutgoingMessages()
	{
		for (auto& flags_record : flags_per_conn)
		{
			auto curr_time = std::chrono::steady_clock::now();
			auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(curr_time - flags_record.second.last_update).count();
			
			if (diff > 100)
			{
				flags_record.second.last_update = curr_time;
				
				json flags_message;
				flags_message["type"] = "flags";
				
				for (auto& flag : flags_record.second.flags)
				{
					flags_message["flags"][flag.name] = flag.state;
					
					if (!flag.toggle)
						flag.state = false;
				}
				
				try {
					net_manager.sendJSONBMessage(flags_record.first, flags_message, {});
				} catch (...)
				{
					v4print "Flags message could not be sent!";
				}
			}
		}
	}
	void handleMessages()
	{
		handleIncomingMessages();
		handleOutgoingMessages();
	}
	void cleanObjects()
	{
		bool destroy_found = false;
		for (const auto& obj : objects_by_id)
		{
			if (obj.second->getDestroy())
			{
				destroy_found = true;
				break;
			}
		}
		
		if (destroy_found)
		{
			unordered_map<ObjectID, shared_ptr<RenderNode>> new_objects_by_id;
			
			for (const auto& obj : objects_by_id)
			{
				if (!obj.second->getDestroy())
				{
					new_objects_by_id[obj.first] = obj.second;
				}
			}
			
			objects_by_id = new_objects_by_id;
		}
	}
	void saveScenes()
	{
		string path = getSaveDir();
		
		string temp_dir = getTempDir();
		
		if (!input_path.empty() && fs::exists(input_path+"/scene_assets"))
		{
			if (fs::exists(path+"/scene_assets"))
			{
				fs::remove_all(path+"/scene_assets");
			}
			fs::copy(input_path+"/scene_assets", path+"/scene_assets");
		}
		else if (fs::exists(temp_dir+"/scene_assets"))
		{
			if (fs::exists(path+"/scene_assets"))
			{
				fs::remove_all(path+"/scene_assets");
			}
			fs::copy(temp_dir+"/scene_assets", path+"/scene_assets");
		}

		json meta_data = {
			{"messages", {}}
		};

		v4print "Saving scenes...";
		static int msg_index = 0;
		for (const auto& window : windows)
		{
			window.second->window->saveSceneArtifacts(path);

			vector<vector<char>> curr_bufs;
			json scene = window.second->produceSceneJSON(&curr_bufs);

			json header = {
				{"type", "scene"},
				{"data", scene}
			};

			string msg_dir = path + "/" + std::to_string(msg_index);
			writeJSONBMessageToFile(header, curr_bufs, msg_dir);
			meta_data["messages"].push_back(msg_index);
			msg_index++;
		}

		ofstream fout(path+"/meta.json");
		fout << meta_data;
		fout.close();
	}
	void saveScreenshots(const string& output_folder)
	{
		int index = 0;
		for (const auto& window : windows)
		{
			auto screenshot = window.second->window->getScreenImage();
			std::string screenshot_path = output_folder + "/window_" + std::to_string(index) + ".png";
			v4print "Saving screenshot to", screenshot_path;
            saveImage(screenshot_path, screenshot);
		}
	}
	void spin(bool daemon, bool debug=false, bool profile=false)
	{
		v4print "Daemon:", daemon;
		
		while (true)
		{
			net_manager.cleanConns();
			
			if (!net_manager.getConnectionIdList().empty())
				had_connection = true;
			else
			if (client_mode && had_connection)
			{
				if (1)
				{
					v4print "Client is reconnecting because all connections closed.";
					objects_by_id.clear();
					for (auto& window : windows)
					{
						window.second->close();
					}
					windows.clear();
					for (auto control_window : control_window_per_conn)
					{
						control_window.second->shutdown();
					}
					control_window_per_conn.clear();
					closed_windows.clear();
					flags_per_conn.clear();
					connectClient(host, port);
					continue;
				}
				else
				{
					v4print "Client is closing because all connections closed.";
					break;
				}
			}
			
			if (windows.empty())
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			
			if (debug)
				v4print "ShowRecord::spin - clean objects";
			cleanObjects();
			
			if (debug)
				v4print "ShowRecord::spin - handle messages";
			handleMessages();
			
			if (debug)
				v4print "ShowRecord::spin - update windows";
			bool save_flag = false;
			for (auto& window : windows)
			{
				window.second->update(debug, profile);
				save_flag |= window.second->save_flag;
				window.second->save_flag = false;
			}
			
			unordered_set<ConnectionID> control_windows_to_remove;
			for (auto control_window : control_window_per_conn)
			{
				if (net_manager.isActiveConn(control_window.first))
				{
					control_window.second->render();
				}
				else
				{
					control_window.second->shutdown();
					control_windows_to_remove.insert(control_window.first);
				}
			}
			for (auto& id : control_windows_to_remove)
			{
				control_window_per_conn.erase(id);
			}

			size_t windows_before = windows.size();
			unordered_map<WindowID, shared_ptr<ShowRecordWindow>> curr_windows;
			for (auto& window : windows)
			{
				if (window.second->shutdown)
				{
					closed_windows.insert(window.first);
				}
				else
				{
					curr_windows[window.first] = window.second;
				}
			}
			windows = curr_windows;
			if (!daemon && !closed_windows.empty())
			{
				v4print "Not a daemon and windows have been closed - exiting.";
				break;
			}

			if (save_flag)
			{
				saveScenes();
			}
			
			if (wait_flag)
			{
				if (windows.size() < windows_before)
				{
					for (auto& window : windows)
					{
						window.second->close();
					}
					wait_flag = false;
					v4print "Wait flag cleared.";
				}
			}

			if (exit_counter > 0)
			{
				exit_counter--;
				if (exit_counter == 0)
					break;
			}
		}
		
		v4print "Exiting main show loop.";
		
		shutdown = true;
		
		if (network_mode)
		{
			v4print "Waiting for thread shutdown.";
			message_thread.join();
		}
	}
};