/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Carnegie Robotics, LLC nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CARNEGIE ROBOTICS, LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

#pragma once

#include <thread>
#include <mutex>
#include <deque>

#include "asset_manager.h"
#include "show_camera.h"
#include "static_camera.h"
#include "spin_camera.h"
#include "trackball_camera.h"
#include "plot_camera.h"

using WindowID = int64_t;
using ObjectID = int64_t;

void waitForMessages(NetworkManager* net_manager, std::deque<JSONBMessage>* messages, std::mutex* message_mutex, bool* shutdown);

struct ShowRecordWindow
{
	shared_ptr<Window> window;
	bool shutdown = false;
	float dt;
	shared_ptr<Texture> text_tex;
	unordered_map<string, shared_ptr<Texture>> texture_bank;
	bool save_flag = false;
	
	// Camera controls
	shared_ptr<ShowCamera> camera;
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
	void addBoundPoint(const Vec3& pt, const TransformSim3& world_from_body);
	void addBoundVerts(const MatXRef& verts, const TransformSim3& world_from_body);
	void addBoundSphere(float rad, const TransformSim3& world_from_body);
	void positionCameraFromObjectBounds();
	void setupCamera(const json& data, AssetManager& assets);
	void setupInputHandlers(NetworkManager* net_manager);
	void refreshKeyMotion();
	shared_ptr<RenderNode> addFromJSON(const json& obj, const vector<vector<char>>& bufs, AssetManager& assets, JSONBMessage& serialization);
	json produceSceneJSON(vector<vector<char>>* bufs = NULL);
	shared_ptr<Texture> loadTexture(const string& path, bool nearest, AssetManager& assets);
};