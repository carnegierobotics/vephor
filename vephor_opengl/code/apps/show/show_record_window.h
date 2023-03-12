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

using WindowID = int;
using ObjectID = int;

void waitForMessages(NetworkManager* net_manager, std::deque<JSONBMessage>* messages, std::mutex* message_mutex, bool* shutdown);

struct ShowRecordWindow
{
	shared_ptr<Window> window;
	bool shutdown = false;
	float dt;
	shared_ptr<Texture> text_tex;
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

	void update();
	void close();
	void setup(const json& data, WindowID p_window_id, ConnectionID p_conn_id, NetworkManager* p_net_manager, AssetManager& assets);
	void addBoundPoint(const Vec3& pt, const TransformSim3& world_from_body);
	void addBoundVerts(const MatXRef& verts, const TransformSim3& world_from_body);
	void addBoundSphere(float rad, const TransformSim3& world_from_body);
	void positionCameraFromObjectBounds();
	void setupCamera(const json& data, AssetManager& assets);
	void setupInputHandlers(NetworkManager* net_manager);
	void refreshKeyMotion();
	shared_ptr<RenderNode> addFromJSON(const json& obj, const vector<vector<char>>& bufs, AssetManager& assets, JSONBMessage& serialization);
	json produceSceneJSON(vector<vector<char>>* bufs = NULL);
};