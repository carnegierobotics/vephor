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

#include <thread>
#include <mutex>
#include <deque>

#include "vephor_ogl.h"

using namespace vephor;

using WindowID = int64_t;
using ObjectID = int64_t;

void waitForMessages(NetworkManager* net_manager, std::deque<JSONBMessage>* messages, std::mutex* message_mutex, bool* shutdown);

struct ShowRecordWindow
{
	shared_ptr<Window> window;
	shared_ptr<RenderNode> on_top_indicator_node;
	bool shutdown = false;
	float dt;
	shared_ptr<Texture> text_tex;
	unordered_map<string, shared_ptr<Texture>> texture_bank;
	bool save_flag = false;
	bool custom_opacity = false;
	
	// Camera controls
	shared_ptr<CameraManager> camera;
	unique_ptr<BoundManager> bound_mgr;

	// Input handling
	ControlInfo control_info;
	
	// Communication
	WindowID window_id;
	ConnectionID conn_id;
	NetworkManager* net_manager = NULL;

	string video_path;
	int video_frame_count = 0;

	void update();
	void close();
	void setup(const json& data, 
		WindowID p_window_id, 
		ConnectionID p_conn_id, 
		NetworkManager* p_net_manager, 
		AssetManager& assets, 
		bool hide_windows);
	void update(const json& data, AssetManager& assets);
	void positionCameraFromObjectBounds();
	void setupCamera(const json& data, AssetManager& assets);
	void setupInputHandlers(NetworkManager* net_manager);
	shared_ptr<RenderNode> addFromJSON(const json& obj, const vector<vector<char>>& bufs, AssetManager& assets, JSONBMessage& serialization);
	json produceSceneJSON(vector<vector<char>>* bufs = NULL);
	shared_ptr<Texture> loadTexture(const string& path, bool nearest, AssetManager& assets);
};