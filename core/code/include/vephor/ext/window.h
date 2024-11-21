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

#include "vephor.h"
#include <random>
#include <filesystem>

namespace fs = std::filesystem;

namespace vephor
{
	
// header: void handleKeyPress(int key_code)
using KeyActionCallback = std::function<void(int)>;

// header: void handleMouseClick(bool left, bool down, const Vec2& pos, const Vec2& window_size)
using MouseClickActionCallback = std::function<void(bool, bool, const Vec2&, const Vec2&)>;
	
// Key codes taken from GLFW
// TODO: make these into constants
/* The unknown key */
const int KEY_UNKNOWN          = -1;

/* Printable keys */
const int KEY_SPACE            = 32;
const int KEY_APOSTROPHE       = 39;  /* ' */
const int KEY_COMMA            = 44;  /* , */
const int KEY_MINUS            = 45;  /* - */
const int KEY_PERIOD           = 46;  /* . */
const int KEY_SLASH            = 47;  /* / */
const int KEY_0                = 48;
const int KEY_1                = 49;
const int KEY_2                = 50;
const int KEY_3                = 51;
const int KEY_4                = 52;
const int KEY_5                = 53;
const int KEY_6                = 54;
const int KEY_7                = 55;
const int KEY_8                = 56;
const int KEY_9                = 57;
const int KEY_SEMICOLON        = 59;  /* ; */
const int KEY_EQUAL            = 61;  /* = */
const int KEY_A                = 65;
const int KEY_B                = 66;
const int KEY_C                = 67;
const int KEY_D                = 68;
const int KEY_E                = 69;
const int KEY_F                = 70;
const int KEY_G                = 71;
const int KEY_H                = 72;
const int KEY_I                = 73;
const int KEY_J                = 74;
const int KEY_K                = 75;
const int KEY_L                = 76;
const int KEY_M                = 77;
const int KEY_N                = 78;
const int KEY_O                = 79;
const int KEY_P                = 80;
const int KEY_Q                = 81;
const int KEY_R                = 82;
const int KEY_S                = 83;
const int KEY_T                = 84;
const int KEY_U                = 85;
const int KEY_V                = 86;
const int KEY_W                = 87;
const int KEY_X                = 88;
const int KEY_Y                = 89;
const int KEY_Z                = 90;
const int KEY_LEFT_BRACKET     = 91;  /* [ */
const int KEY_BACKSLASH        = 92;  /* \ */
const int KEY_RIGHT_BRACKET    = 93;  /* ] */
const int KEY_GRAVE_ACCENT     = 96;  /* ` */
const int KEY_WORLD_1          = 161; /* non-US #1 */
const int KEY_WORLD_2          = 162; /* non-US #2 */

/* Function keys */
const int KEY_ESCAPE           = 256;
const int KEY_ENTER            = 257;
const int KEY_TAB              = 258;
const int KEY_BACKSPACE        = 259;
const int KEY_INSERT           = 260;
const int KEY_DELETE           = 261;
const int KEY_RIGHT            = 262;
const int KEY_LEFT             = 263;
const int KEY_DOWN             = 264;
const int KEY_UP               = 265;
const int KEY_PAGE_UP          = 266;
const int KEY_PAGE_DOWN        = 267;
const int KEY_HOME             = 268;
const int KEY_END              = 269;
const int KEY_CAPS_LOCK        = 280;
const int KEY_SCROLL_LOCK      = 281;
const int KEY_NUM_LOCK         = 282;
const int KEY_PRINT_SCREEN     = 283;
const int KEY_PAUSE            = 284;
const int KEY_F1               = 290;
const int KEY_F2               = 291;
const int KEY_F3               = 292;
const int KEY_F4               = 293;
const int KEY_F5               = 294;
const int KEY_F6               = 295;
const int KEY_F7               = 296;
const int KEY_F8               = 297;
const int KEY_F9               = 298;
const int KEY_F10              = 299;
const int KEY_F11              = 300;
const int KEY_F12              = 301;
const int KEY_F13              = 302;
const int KEY_F14              = 303;
const int KEY_F15              = 304;
const int KEY_F16              = 305;
const int KEY_F17              = 306;
const int KEY_F18              = 307;
const int KEY_F19              = 308;
const int KEY_F20              = 309;
const int KEY_F21              = 310;
const int KEY_F22              = 311;
const int KEY_F23              = 312;
const int KEY_F24              = 313;
const int KEY_F25              = 314;
const int KEY_KP_0             = 320;
const int KEY_KP_1             = 321;
const int KEY_KP_2             = 322;
const int KEY_KP_3             = 323;
const int KEY_KP_4             = 324;
const int KEY_KP_5             = 325;
const int KEY_KP_6             = 326;
const int KEY_KP_7             = 327;
const int KEY_KP_8             = 328;
const int KEY_KP_9             = 329;
const int KEY_KP_DECIMAL       = 330;
const int KEY_KP_DIVIDE        = 331;
const int KEY_KP_MULTIPLY      = 332;
const int KEY_KP_SUBTRACT      = 333;
const int KEY_KP_ADD           = 334;
const int KEY_KP_ENTER         = 335;
const int KEY_KP_EQUAL         = 336;
const int KEY_LEFT_SHIFT       = 340;
const int KEY_LEFT_CONTROL     = 341;
const int KEY_LEFT_ALT         = 342;
const int KEY_LEFT_SUPER       = 343;
const int KEY_RIGHT_SHIFT      = 344;
const int KEY_RIGHT_CONTROL    = 345;
const int KEY_RIGHT_ALT        = 346;
const int KEY_RIGHT_SUPER      = 347;
const int KEY_MENU             = 348;


// In case we want to consider a window plugin scheme in the future:
/*class WindowPlugin
{
public:
	virtual json serialize() = 0;
};
	
template <class T>
shared_ptr<WindowPlugin> getWindowPlugin(const shared_ptr<T>& obj)
{
	throw std::runtime_error("Could not create window plugin for unsupported object type: "+string(typeid(*obj.get()).name()));
	return NULL;
};*/

template <typename T>
std::string formatDecimal(T d, int decimals)
{
    std::stringstream sstr;
    sstr.precision(decimals);
    sstr << std::fixed << d;
    return sstr.str();
}

// ChatGPT
inline std::string formatByteDisplay(double bytes, int unit = 0)
{
	std::vector<std::string> units = { "bytes", "KB", "MB", "GB", "TB", "PB" };

	if (bytes < 1024.0 || unit >= units.size() - 1) {
		return formatDecimal(bytes, 2) + " " + units[unit];
	}
	else {
		return formatByteDisplay(bytes / 1024.0, unit + 1);
	}
}

class Window;
using ObjectID = int;

const ConnectionID LOCAL_CONN_ID_PROGRESSIVE = std::numeric_limits<ConnectionID>::max() - 1;
const ConnectionID LOCAL_CONN_ID_ABSOLUTE = std::numeric_limits<ConnectionID>::max();

struct RenderNode
{
public:
	friend Window;

	RenderNode(const shared_ptr<TransformNode>& p_node):node(p_node){}
	virtual ~RenderNode()
	{
	}
	virtual json serialize(ConnectionID conn_id, vector<vector<char>>* bufs) = 0;
	Vec3 getPos() const
	{
		return node->getPos();
	}
	RenderNode* setPos(const Vec3& pos)
	{
		node->setPos(pos);
		return this;
	}
	Orient3 getOrient() const
	{
		return node->getOrient();
	}
	RenderNode* setOrient(const Orient3& orient)
	{
		node->setOrient(orient);
		return this;
	}
	TransformSim3 getTransform() const
	{
		return node->getTransform();
	}
	RenderNode* setTransform(const TransformSim3& t)
	{
		node->setTransform(t);
		return this;
	}
	float getScale() const
	{
		return node->getScale();
	}
	RenderNode* setScale(float scale)
	{
		node->setScale(scale);
		return this;
	}
	RenderNode* setParent(TransformNode& parent)
	{
		if (node->getParent() != NULL)
			throw std::runtime_error("RenderNode already has a parent.");
		parent.addChild(node);
		return this;
	}
	RenderNode* setParent(RenderNode& parent)
	{
		if (node->getParent() != NULL)
			throw std::runtime_error("RenderNode already has a parent.");
		parent.node->addChild(node);
		return this;
	}
	RenderNode* setParent(const shared_ptr<TransformNode>& parent)
	{
		setParent(*parent.get());
		return this;
	}
	RenderNode* setParent(const shared_ptr<RenderNode>& parent)
	{
		setParent(*parent.get());
		return this;
	}
	RenderNode* setShow(bool p_show)
	{
		show = p_show;
		for (auto& status : net_status)
		{
			status.second.show_up_to_date = false;
		}
		return this;
	}
	bool getShow() const {return show;}
	RenderNode* setDestroy() {
		destroy = true;
		for (auto& status : net_status)
		{
			status.second.destroy_up_to_date = false;
		}
		return this;
	}
	bool getDestroy() const {return destroy;}	
	ObjectID getID() const {return id;}
	bool isNetworkUpToDate() const
	{
		// TODO: should this be looking for connections this object hasn't listed?
		for (auto& status : net_status)
		{
			if (!(status.second.obj_up_to_date && 
				status.second.pose_up_to_date && 
				status.second.show_up_to_date && 
				status.second.destroy_up_to_date))
				return false;
		}
		return true;
	}
private:
	ObjectID id = -1;
	bool on_overlay;
	int layer;
	shared_ptr<TransformNode> node;
	
	bool show = true;
	bool destroy = false;
	
	struct RenderObjectNetworkStatus
	{
		bool obj_up_to_date = false;
		bool pose_up_to_date = false;
		bool show_up_to_date = false;
		bool destroy_up_to_date = false;
	};
	
	unordered_map<ConnectionID, RenderObjectNetworkStatus> net_status;
};

using WindowID = int;
using ObjectID = int;

struct ShowFlag
{
	bool toggle;
	bool state = false;
};

struct ShowMetadata
{
	string app_name;
	unordered_map<string, ShowFlag> flags;
	
	json serialize()
	{
		json data;
		data["type"] = "metadata";
		data["name"] = app_name;
		for (const auto& flag : flags)
		{
			data["flags"].push_back({
				{"name",flag.first},
				{"toggle",flag.second.toggle},
				{"state",flag.second.state}
			});
		}
		return data;
	}
};

struct WindowManager
{
	enum Mode
	{
		File,
		Record,
		Server,
		ServerBYOC,
		Client,
		ClientBYOS
	};

	Mode mode = Mode::File;
	WindowID next_window_id = 0;
	ObjectID next_obj_id = 0;
	unordered_map<ConnectionID, unordered_set<string>> transferred_files;
	NetworkManager net;
	bool network_mode = false;
	unordered_map<WindowID, vector<json>> window_messages;
	bool first_render = false;
	ShowMetadata show_metadata;
	unordered_map<ConnectionID, bool> metadata_up_to_date;
	
	void checkIncomingMessages()
	{
		// Sort through all incoming messages and distribute them to windows
		for (auto id : net.getConnectionIdList())
		{
			for (const auto& msg : net.getIncomingJSONBMessages(id))
			{
				if (msg.header["type"] == "flags")
				{
					for (const auto& [key, value] : msg.header["flags"].items())
					{
						if (show_metadata.flags[key].toggle)
							show_metadata.flags[key].state = value;
						else
						{
							if (value)
								show_metadata.flags[key].state = true;
						}
					}
					continue;
				}
				
				WindowID window_id = 0;
				if (msg.header.contains("window"))
					window_id = msg.header["window"];
				window_messages[window_id].push_back(msg.header);
			}
		}
	}
	vector<json> getWindowMessages(WindowID id)
	{
		checkIncomingMessages();
		vector<json> msgs = window_messages[id];
		window_messages[id].clear();
		return msgs;
	}
	void updateMetadata()
	{
		for (auto conn_id : net.getConnectionIdList())
		{
			if (!show_metadata.flags.empty() && (!find(metadata_up_to_date, conn_id) || !metadata_up_to_date[conn_id]))
			{
				JSONBMessage msg;
				msg.header = show_metadata.serialize();
				v4print "Sending metadata.";
				if (!net.sendJSONBMessage(conn_id, msg.header, msg.payloads))
					v4print "Failed to send metadata to connection", conn_id;
				metadata_up_to_date[conn_id] = true;
			}
		}
	}
};

class Window {
public:
    explicit Window(float p_width = -1, float p_height = -1, const string &p_title = "show")
            : Window(/* p_width */ p_width,
                     /* p_height */ p_height,
                     /* p_x_position */ -1,
                     /* p_y_position */ -1,
                     /* p_title */ p_title)
    {
	}

    Window(float p_width,
           float p_height,
           float p_x_position,
           float p_y_position = -1,
           const string &p_title = "show")
            : width(p_width), height(p_height), x_position(p_x_position), y_position(p_y_position), title(p_title)
	{
		opacity = default_opacity;
		
		id = manager.next_window_id;
		manager.next_window_id++;
		
		window_top_right_node = make_shared<TransformNode>(Transform3());
		window_top_right_node->setName("window_top_right");
		
		window_bottom_right_node = make_shared<TransformNode>(Transform3());
		window_bottom_right_node->setName("window_bottom_right");
		
		window_top_left_node = make_shared<TransformNode>(Transform3());
		window_top_left_node->setName("window_top_left");
		
		window_bottom_left_node = make_shared<TransformNode>(Transform3());
		window_bottom_left_node->setName("window_bottom_left");
		
		camera_control = {
			{"type", "trackball"},
			{"to", {0,0,0}},
			{"from", {-15,0,-15}},
			{"up", {0,0,-1}},
			{"3d", false}		
		};
	}

    ~Window() = default;

	void layoutAbsolute(
		float p_width,
		float p_height,
		float p_x_position = -1,
		float p_y_position = -1
	)
	{
		width = p_width;
		height = p_height;
		x_position = p_x_position;
		y_position = p_y_position;
		perunit_layout = false;
	}

	void layoutPerUnit(
		float p_width,
		float p_height,
		float p_x_position = -1,
		float p_y_position = -1
	)
	{
		width = p_width;
		height = p_height;
		x_position = p_x_position;
		y_position = p_y_position;
		perunit_layout = true;
	}

	void setTitle(const string& p_title)
	{
		title = p_title;
		if (camera_control["type"] == "plot")
		{
			camera_control["title"] = title;
		}
		invalidateCameraControlInfo();
	}

	void setFrameLock(float p_fps){fps = p_fps;}

	void setOpacity(float p_opacity)
	{
		opacity = p_opacity;
		invalidateCameraControlInfo();
	}

	void setFrameSkipMessageLimit(int p_frame_skip_message_limit){frame_skip_message_limit = p_frame_skip_message_limit;}

    void setStaticCameraMode(const Vec3 &to = {0, 0, 0},
                             const Vec3 &from = {-1, 0, -1},
                             const Vec3 &up = {0, 0, -1})
    {
        camera_control = {
                {"type", "static"},
                {"to",   toJson(to)},
                {"from", toJson(from)},
                {"up",   toJson(up)},
        };
    }

	void setTrackballMode(const Vec3& to = Vec3(0,0,0), const Vec3& from = Vec3(-1,0,-1), const Vec3& up = Vec3(0,0,-1), bool use_3d = false)
	{
		camera_control = {
			{"type", "trackball"},
			{"to", toJson(to)},
			{"from", toJson(from)},
			{"up", toJson(up)},
			{"3d", use_3d}
		};
	}

	void setTrackballModeVision()
	{
		setTrackballMode(Vec3::Zero(), Vec3(-1,-1,0), Vec3(0,-1,0));
	}
	
	void setPlotMode(bool equal = false)
	{
		camera_control = {
			{"type", "plot"},
			{"equal", equal},
			{"y_flip", false}
		};
	}

	void setPlot3DMode(const Vec3& to = Vec3(0,0,0), const Vec3& from = Vec3(-1,0,-1), const Vec3& up = Vec3(0,0,-1))
	{
		camera_control = {
			{"type", "plot3d"},
			{"to", toJson(to)},
			{"from", toJson(from)},
			{"up", toJson(up)}
		};
	}

	json& getCameraControlInfo()
	{
		return camera_control;
	}
	
	void setKeyPressCallback(KeyActionCallback p_callback)
    {
        key_press_callback = p_callback;
    }

	void setMouseClickCallback(MouseClickActionCallback p_callback)
    {
        mouse_click_callback = p_callback;
    }
	
	shared_ptr<RenderNode> add(
		const TransformSim3& parent_from_node
	)
	{
		auto node = make_shared<TransformNode>(parent_from_node);
		shared_ptr<RenderNode> inner_obj = make_shared<NullRenderNode>(node);
		inner_obj->id = manager.next_obj_id;
		manager.next_obj_id++;
		node->setName(to_string(inner_obj->id));
		node->setScale(parent_from_node.scale);
		objects.push_back(inner_obj);

		auto* inner_obj_ptr = inner_obj.get();
		
		node->addTransformCallback([inner_obj_ptr](const TransformSim3&)
		{
			for (auto& status : inner_obj_ptr->net_status)
				status.second.pose_up_to_date = false;
		});
		
		return inner_obj;
	}
	
	shared_ptr<RenderNode> add(
		const Vec3& parent_from_node_t,
		const Vec3& parent_from_node_r = Vec3::Zero(),
		float parent_from_node_scale = 1.0f
	)
	{
		return add(TransformSim3(parent_from_node_t,parent_from_node_r,parent_from_node_scale));
	}
	
	template <class T>
    shared_ptr<RenderNode> add(
		const shared_ptr<T>& obj,
		const TransformSim3& parent_from_node = TransformSim3(),
		bool on_overlay = false, 
		int layer = 0
	)
    {
		auto node = make_shared<TransformNode>(parent_from_node);
		shared_ptr<RenderNode> inner_obj = make_shared<TRenderNode<T>>(obj, node);
		inner_obj->id = manager.next_obj_id;
		manager.next_obj_id++;
		node->setName(to_string(inner_obj->id));
		node->setScale(parent_from_node.scale);
		inner_obj->on_overlay = on_overlay;
		inner_obj->layer = layer;
		objects.push_back(inner_obj);

		auto* inner_obj_ptr = inner_obj.get();
		
		node->addTransformCallback([inner_obj_ptr](const TransformSim3&)
		{
			for (auto& status : inner_obj_ptr->net_status)
				status.second.pose_up_to_date = false;
		});
		
		return inner_obj;
    }
	
	template <class T>
    shared_ptr<RenderNode> add(
		const shared_ptr<T>& obj, 
		const Vec3& parent_from_node_t,
		const Vec3& parent_from_node_r = Vec3::Zero(),
		float parent_from_node_scale = 1.0f,
		bool on_overlay = false, 
		int layer = 0
	)
    {
		return add(obj, TransformSim3(parent_from_node_t,parent_from_node_r,parent_from_node_scale), on_overlay, layer);
    }
	
	shared_ptr<TransformNode> getWindowTopRightNode() const {return window_top_right_node;}
	shared_ptr<TransformNode> getWindowBottomRightNode() const {return window_bottom_right_node;}
	shared_ptr<TransformNode> getWindowTopLeftNode() const {return window_top_left_node;}
	shared_ptr<TransformNode> getWindowBottomLeftNode() const {return window_bottom_left_node;}

	void setPrintFlagNetworkUse(bool flag = true){print_flag_network_use = flag;}

private:
	bool print_flag_network_use = false;

	int writeMessages(
		const string& path, 
		const vector<JSONBMessage>& messages_to_write, 
		bool skip_meta = false, 
		int start_msg_index = 0)
	{
		json meta_data = {
			{"messages", {}}
		};

		int msg_index = start_msg_index;
		for (const auto& msg : messages_to_write)
		{
			string msg_dir = path + "/" + std::to_string(msg_index);

			writeJSONBMessageToFile(msg.header, msg.payloads, msg_dir);

			meta_data["messages"].push_back(msg_index);

			msg_index++;
		}

		if (!skip_meta)
		{
			ofstream fout(path+"/meta.json");
			fout << meta_data;
			fout.close();
		}

		return msg_index;
	}

public:
	void processEvents(bool& key_event, bool& hide_event, bool verbose = false)
	{
		key_event = false;
		hide_event = false;

		if (!manager.network_mode)
			return;

		auto msgs = manager.getWindowMessages(id);

		if (verbose)
			v4print "Processing window event messages:", msgs.size();
		
		for (const auto& msg : msgs)
		{
			if (msg["type"] == "key_press")
			{	
				if (key_press_callback)
				{
					key_press_callback(msg["key"]);
				}
				
				if (msg["key"] == KEY_ENTER)
				{
					key_event = true;
					v4print "Continue key event received for window:", id;
				}
			}
			else if (msg["type"] == "mouse_click")
			{
				if (mouse_click_callback)
				{
					mouse_click_callback(
						msg["button"] == "left", 
						msg["state"] == "down", 
						readVec2(msg["pos"]),
						readVec2(msg["window_size"])
					);
				}
			}
			else if (msg["type"] == "close")
			{
				v4print "Close message received for window:", id;
				shutdown = true;
			}\
			else if (msg["type"] == "hide")
			{
				v4print "Hide message received for window:", id;
				hide_event = true;
			}
		}
	}
	bool render(bool wait_close = true, bool wait_key = false, float time_increment_s = 0.0f)
	{
		manager.first_render = true;

		if (shutdown)
			return false;
		
		if (manager.mode == WindowManager::Mode::Record)	
		{
			JSONBMessage msg;
			json scene = produceSceneJSON(LOCAL_CONN_ID_PROGRESSIVE, &msg.payloads);
			auto end_time = std::chrono::high_resolution_clock::now();

			float record_time;
			if (time_increment_s > 0)
				record_time = last_record_time + time_increment_s;
			else
				record_time = std::chrono::duration<float, std::milli>(end_time-record_start_time).count() / 1000.0f;

			msg.header = {
				{"type", "scene"},
				{"data", scene}
			};
			msg.header["time"] = record_time;
			msg.valid = true;

			last_record_time = record_time;

			vector<JSONBMessage> messages_to_write = {msg};
			if (!find(recorded_messages_written, LOCAL_CONN_ID_PROGRESSIVE))
				recorded_messages_written[LOCAL_CONN_ID_PROGRESSIVE] = 0;
			recorded_messages_written[LOCAL_CONN_ID_PROGRESSIVE] = writeMessages(
				record_path, 
				messages_to_write, 
				true, 
				recorded_messages_written[LOCAL_CONN_ID_PROGRESSIVE]);

			// TODO: update incrementally
			string temp_dir = getTempDir();
			if (fs::exists(temp_dir+"/scene_assets"))
			{
				if (fs::exists(record_path+"/scene_assets"))
				{
					fs::remove_all(record_path+"/scene_assets");
				}
				fs::copy(temp_dir+"/scene_assets", record_path+"/scene_assets");
			}
		}
		else if (manager.network_mode)
		{
			manager.net.cleanConns();

			string temp_dir = getTempDir();
			string asset_dir = temp_dir + "/scene_assets";
			
			for (auto conn_id : manager.net.getConnectionIdList())
			{
				manager.updateMetadata();


				if (frame_skip_message_limit >= 0)
				{
					int q_size = manager.net.getJSONBOutgoingQueueSize(conn_id);
					if (frame_messages_waiting > frame_skip_message_limit)
					{
						if (frame_message_skips % 100 == 0)
							v4print "Skipping frame due to unsent outgoing messages - Window:", id, "Title:", title, "Conn:", conn_id, "Queue Size:", q_size, "Frame Messages Waiting:", frame_messages_waiting;
						frame_message_skips++;
						continue;
					}
				}
				
				JSONBMessage msg;
				json scene_data = produceSceneJSON(conn_id, &msg.payloads);
				
				if (fs::exists(asset_dir))
				{
					for (const auto & entry : fs::directory_iterator(asset_dir))
					{
						if (find(manager.transferred_files[conn_id], entry.path().string()))
							continue;
						
						v4print "File:", entry.path();

						std::ifstream file(entry.path(), std::ios::binary | std::ios::ate);
						std::streamsize size = file.tellg();
						file.seekg(0, std::ios::beg);

						vector<char> buffer(size);
						if (file.read(buffer.data(), size))
						{
							v4print "File size:", buffer.size();
						}
						else
							throw std::runtime_error("Error reading file data.");
						
						json file_name_message = {
							{"type", "file"},
							{"name", "scene_assets/"+entry.path().filename().string()}
						};
						manager.net.sendJSONBMessage(conn_id, file_name_message, {std::move(buffer)});
						
						manager.transferred_files[conn_id].insert(entry.path().string());
					}
				}
				
				msg.header = {
					{"type", "scene"},
					{"data", scene_data}
				};

				if (print_flag_network_use)
				{
					auto msg_size = msg.getSize();
					total_network_use_bytes += msg_size;

					if (!network_use_start_time_set)
					{
						network_use_start_time_set = true;
						network_use_start_time = std::chrono::high_resolution_clock::now();
					}
					auto curr_time = std::chrono::high_resolution_clock::now();
					float network_use_time = std::chrono::duration<float, std::milli>(curr_time-network_use_start_time).count() / 1000.0f;
					if (network_use_time < 1.0f)
						network_use_time = 1.0f;
					v4print "Network use - Connection:", conn_id, 
						"Time:", formatDecimal(network_use_time, 2),
						"Data:", formatByteDisplay(msg_size), 
						"Rate:", formatByteDisplay(total_network_use_bytes / network_use_time), "/ s";
				}

				frame_messages_waiting++;
				manager.net.sendJSONBMessage(conn_id, msg.header, msg.payloads, [&](const shared_ptr<JSONBMessage>& /*msg*/){
					frame_messages_waiting--;
				});

				// Record after sending so time doesn't go out in the network message
				if (!record_path.empty())
				{
					string conn_record_dir = record_path + "/conn_" + std::to_string(conn_id);

					if (!fs::exists(conn_record_dir))
						fs::create_directories(conn_record_dir);

					auto end_time = std::chrono::high_resolution_clock::now();
					float record_time = std::chrono::duration<float, std::milli>(end_time-record_start_time).count() / 1000.0f;

					msg.header["time"] = record_time;

					vector<JSONBMessage> messages_to_write = {msg};

					if (!find(recorded_messages_written, conn_id))
						recorded_messages_written[conn_id] = 0;
					recorded_messages_written[conn_id] = writeMessages(
						conn_record_dir, 
						messages_to_write, 
						true, 
						recorded_messages_written[conn_id]);

					// TODO: update incrementally
					string temp_dir = getTempDir();
					if (fs::exists(temp_dir+"/scene_assets"))
					{
						if (fs::exists(conn_record_dir+"/scene_assets"))
						{
							fs::remove_all(conn_record_dir+"/scene_assets");
						}
						fs::copy(temp_dir+"/scene_assets", conn_record_dir+"/scene_assets");
					}
				}

				

				if (wait_close || wait_key)
				{
					json show = {
						{"type", "wait"}
					};
					manager.net.sendJSONBMessage(conn_id, show, {});
				}
			}
			
			// Handle incoming messages
			bool keep_waiting = false;
			if (wait_close || wait_key)
			{
				keep_waiting = true;
			}
			while (true)
			{
				bool key_event = false;
				bool hide_event = false;
				processEvents(key_event, hide_event);

				if (wait_key && key_event)
					keep_waiting = false;

				if (wait_close && hide_event)
					keep_waiting = false;
				
				if (!keep_waiting || shutdown)
					break;

				std::this_thread::sleep_for(std::chrono::milliseconds(100));

				json heartbeat = {
					{"type", "heartbeat"}
				};
				if (id > 0)
					heartbeat["window_id"] = id;
				
				if (!manager.net.sendJSONBMessageToAll(heartbeat, {}))
					break;
			}
		}
		else
		{
			string temp_dir = getTempDir();
			
			JSONBMessage msg;
			json scene = produceSceneJSON(LOCAL_CONN_ID_PROGRESSIVE, &msg.payloads);
			msg.header = {
				{"type", "scene"},
				{"data", scene}
			};
			msg.valid = true;
			recorded_messages_to_write.push_back(msg);

			if (wait_close || wait_key)
			{
				if (!find(recorded_messages_written, LOCAL_CONN_ID_PROGRESSIVE))
					recorded_messages_written[LOCAL_CONN_ID_PROGRESSIVE] = 0;
				recorded_messages_written[LOCAL_CONN_ID_PROGRESSIVE] = writeMessages(
					temp_dir, 
					recorded_messages_to_write, 
					true, 
					recorded_messages_written[LOCAL_CONN_ID_PROGRESSIVE]);

				string show_path = getBaseDir()+"/bin/vephor_show";
				if (!fs::exists(show_path))
					show_path = "vephor_show"; // Hope it is on PATH
				v4print "Calling show at path:", show_path;
				Process proc({show_path, "-i", temp_dir, "-r"});
				int result = proc.join();
				if (result != 0)
					v4print "Show exited without success.";
				else
					v4print "Show exited with success.";

				recorded_messages_to_write.clear();
			}
			else
			{
				v4print "Waiting to render until called with wait=true.";
			}
		}

		// Remove destroyed objects
		size_t obj_index = 0;
		size_t removed_objs = 0;
		while (obj_index < objects.size() - removed_objs)
		{
			auto& obj = objects[obj_index];
			if (obj->destroy && obj->isNetworkUpToDate())
			{
				removed_objs++;
				obj = objects[objects.size() - removed_objs];
				continue;
			}
			obj_index++;
		}
		objects.resize(objects.size() - removed_objs);
		
		return !shutdown;
	}

	void save(string path)
	{
		path = fs::absolute(path).string();

		v4print "Saving scene to", path;
		
		string temp_dir = getTempDir();
		
		JSONBMessage msg;
		json scene = produceSceneJSON(LOCAL_CONN_ID_ABSOLUTE, &msg.payloads);
		msg.header = {
			{"type", "scene"},
			{"data", scene}
		};
		msg.valid = true;

		vector<JSONBMessage> messages_to_write = {msg};
		writeMessages(path, messages_to_write);

		if (fs::exists(temp_dir+"/scene_assets"))
		{
			v4print "Copying scene assets from", temp_dir+"/scene_assets", "to", path+"/scene_assets";
			fs::remove_all(path+"/scene_assets");
			fs::copy(temp_dir+"/scene_assets", path+"/scene_assets");
		}
	}

	static void setGlobalDefaultOpacity(float p_opacity)
	{
		default_opacity = p_opacity;
	}
	
	static bool checkAndConsumeFlag(const string& flag)
	{
		if (!manager.network_mode)
			return false;

		manager.updateMetadata();
		manager.checkIncomingMessages();
		
		if (!find(manager.show_metadata.flags, flag))
			return false;
		if (manager.show_metadata.flags[flag].toggle)
			return manager.show_metadata.flags[flag].state;
		bool ret_val = manager.show_metadata.flags[flag].state;
		manager.show_metadata.flags[flag].state = false;
		return ret_val;
	}

	static bool canRender()
	{
		if (manager.mode == WindowManager::Mode::Client || 
			manager.mode == WindowManager::Mode::Server)
			return !manager.net.getConnectionIdList().empty();
		return true;
	}

	static void setRecordMode(const string& path = "")
	{
		if (manager.mode == WindowManager::Mode::Record)
			return;

		if (manager.mode != WindowManager::Mode::File)
			throw std::runtime_error("Window mode already set.");

		record_path = path;
		if (record_path.empty())
		{
			record_path = getSaveDir();
		}
		else 
		{
			if (fs::exists(record_path))
			{
				throw std::runtime_error("Record directory exists.");
			}
			fs::create_directories(record_path);
		}

		record_start_time = std::chrono::high_resolution_clock::now();

		manager.mode = WindowManager::Mode::Record;
	}
	
	static void setClientMode(
		bool wait = false, 
		const string& host = "localhost", 
		int port = VEPHOR_DEFAULT_PORT,
		bool p_record_also = false, 
		const string p_record_path = "")
	{
		if (manager.mode == WindowManager::Mode::Client)
			return;

		if (manager.mode != WindowManager::Mode::File)
			throw std::runtime_error("Window mode already set.");

		if (manager.first_render)
			throw std::runtime_error("Must set window mode before first render.");

		manager.network_mode = true;
		v4print "Connecting client...";
		manager.net.connectClient(wait, host, port);
		v4print "Client connected.";

		if (p_record_also)
		{
			if (!p_record_path.empty())
			{
				record_path = p_record_path;
			}
			else
			{
				record_path = getSaveDir();
			}
			record_start_time = std::chrono::high_resolution_clock::now();
		}

		manager.mode = WindowManager::Mode::Client;
	}
	
	static void setClientModeBYOS(bool p_record_also = false, const string p_record_path = "")
	{
		if (manager.mode == WindowManager::Mode::ClientBYOS)
			return;

		if (manager.mode != WindowManager::Mode::File)
			throw std::runtime_error("Window mode already set.");

		if (manager.first_render)
			throw std::runtime_error("Must set window mode before first render.");

		if (server_proc)
			throw std::runtime_error("Server process already started.");

		std::random_device dev;
    	std::mt19937 rng(dev());
    	std::uniform_int_distribution<std::mt19937::result_type> port_dist(VEPHOR_MIN_RANDOM_PORT,VEPHOR_MAX_RANDOM_PORT);

		int port = port_dist(rng);
		v4print "Using port:", port;
	
		v4print "Starting server process...";
		server_proc = make_unique<Process>(vector<string>{"vephor_show", 
			"-m", "server", 
			"-o", "localhost", 
			"-p", std::to_string(port),
			"-r"});
		v4print "Server process started.";
		
		manager.network_mode = true;
		v4print "Connecting client...";
		while (true)
		{
			try {
				manager.net.connectClient(true, "localhost", port);
				break;
			} catch (...) {}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		v4print "Client connected.";

		if (p_record_also)
		{
			if (!p_record_path.empty())
			{
				record_path = p_record_path;
			}
			else
			{
				record_path = getSaveDir();
			}
			record_start_time = std::chrono::high_resolution_clock::now();
		}

		manager.mode = WindowManager::Mode::ClientBYOS;
	}
	
	static void setServerMode(
		bool wait = false, 
		int port = VEPHOR_DEFAULT_PORT, 
		bool p_record_also = false, 
		const string p_record_path = "",
		const ShowMetadata& p_show_metadata = ShowMetadata())
	{
		if (manager.mode == WindowManager::Mode::Server)
			return;

		if (manager.mode != WindowManager::Mode::File)
			throw std::runtime_error("Window mode already set.");

		if (manager.first_render)
			throw std::runtime_error("Must set window mode before first render.");

		manager.network_mode = true;
		manager.net.connectServer(wait, port);

		if (p_record_also)
		{
			if (!p_record_path.empty())
			{
				record_path = p_record_path;
			}
			else
			{
				record_path = getSaveDir();
			}
			record_start_time = std::chrono::high_resolution_clock::now();
		}
		
		manager.show_metadata = p_show_metadata;
		manager.mode = WindowManager::Mode::Server;
	}

	static void setServerModeBYOC(bool p_record_also = false, const string p_record_path = "")
	{
		if (manager.mode == WindowManager::Mode::ServerBYOC)
			return;

		if (manager.mode != WindowManager::Mode::File)
			throw std::runtime_error("Window mode already set.");

		if (manager.first_render)
			throw std::runtime_error("Must set window mode before first render.");

		if (server_proc)
			throw std::runtime_error("Server process already started.");

		std::random_device dev;
    	std::mt19937 rng(dev());
    	std::uniform_int_distribution<std::mt19937::result_type> port_dist(VEPHOR_MIN_RANDOM_PORT,VEPHOR_MAX_RANDOM_PORT);

		int port;
		
		while (true)
		{
			port = port_dist(rng);
			v4print "Using port:", port;

			if (manager.net.connectServer(false, port))
				break;

			v4print "Bind failed, tryin another port.";
		}
	
		v4print "Starting client process...";
		server_proc = make_unique<Process>(vector<string>{"vephor_show", 
			"-m", "client", 
			"-o", "localhost", 
			"-p", std::to_string(port),
			"-r",
			"-d"});
		v4print "Client process started.";
		
		manager.network_mode = true;
		
		while (manager.net.getConnectionIdList().empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		if (p_record_also)
		{
			if (!p_record_path.empty())
			{
				record_path = p_record_path;
			}
			else
			{
				record_path = getSaveDir();
			}
			record_start_time = std::chrono::high_resolution_clock::now();
		}

		manager.mode = WindowManager::Mode::ServerBYOC;
	}

	void clear()
	{
		for (auto& obj : objects)
		{
			obj->setDestroy();
		}
	}

	void invalidateCameraControlInfo()
	{
		for (auto& up_to_date : camera_up_to_date)
			up_to_date.second = false;
	}
private:
	json produceSceneJSON(ConnectionID conn_id = std::numeric_limits<ConnectionID>::max(), vector<vector<char>>* bufs = NULL)
	{
		json scene;
		
		if (id > 0)
			scene["window_id"] = id;
		
		if (!find(camera_up_to_date, conn_id) || !camera_up_to_date[conn_id])
		{
			scene["window"] = {
				{"width", width},
				{"height", height},
				{"x_position", x_position},
				{"y_position", y_position},
				{"perunit_layout", perunit_layout},
				{"title", title},
				{"fps", fps},
				{"opacity", opacity}
			};
			scene["camera"]["control"] = camera_control;

			if (conn_id != LOCAL_CONN_ID_ABSOLUTE)
				camera_up_to_date[conn_id] = true;
		}
		
		scene["objects"] = json();
		
		for (const auto& obj : objects)
		{
			if (conn_id == LOCAL_CONN_ID_ABSOLUTE)
			{
				if (obj->destroy)
					continue;

				json datum = obj->serialize(conn_id, bufs);
				datum["id"] = obj->id;
				datum["show"] = obj->show;
				datum["overlay"] = obj->on_overlay;
				datum["layer"] = obj->layer;
				scene["objects"].push_back(datum);

				continue;
			}

			if (
				obj->net_status[conn_id].obj_up_to_date && 
				obj->net_status[conn_id].pose_up_to_date && 
				obj->net_status[conn_id].show_up_to_date && 
				obj->net_status[conn_id].destroy_up_to_date)
				continue;
			
			json datum = obj->serialize(conn_id, bufs);
			datum["id"] = obj->id;
			
			if (!obj->net_status[conn_id].show_up_to_date)
			{
				datum["show"] = obj->show;
			}
			
			if (!obj->net_status[conn_id].destroy_up_to_date && obj->destroy)
			{
				datum["destroy"] = true;
			}
			
			if (!obj->net_status[conn_id].obj_up_to_date)
			{
				datum["overlay"] = obj->on_overlay;
				datum["layer"] = obj->layer;
			}
			scene["objects"].push_back(datum);
			
			obj->net_status[conn_id].obj_up_to_date = true;
			obj->net_status[conn_id].pose_up_to_date = true;
			obj->net_status[conn_id].show_up_to_date = true;
			obj->net_status[conn_id].destroy_up_to_date = true;
		}

		return scene;
	}

	struct NullRenderNode : public RenderNode
    {
    public:
        NullRenderNode(const shared_ptr<TransformNode>& p_node)
        : RenderNode(p_node)
        {}

		~NullRenderNode()
		{
		}

        virtual json serialize(ConnectionID conn_id, vector<vector<char>>*) override
        {
			json data;
			if (!net_status[conn_id].obj_up_to_date)
			{
				data["type"] = "null";
				if (node->getParent())
				{
					if (!node->getParent()->getName().empty())
					{
						data["pose_parent"] = node->getParent()->getName();
					}
				}
			}
			if (!net_status[conn_id].pose_up_to_date)
			{
				data["pose"] = toJson(node->getTransform());
			}
            return data;
        }
    };

	template <class T>
    struct TRenderNode : public RenderNode
    {
    public:
        TRenderNode(const shared_ptr<T>& p_obj, const shared_ptr<TransformNode>& p_node)
        : RenderNode(p_node), obj(p_obj)
        {}

		~TRenderNode()
		{
		}

        virtual json serialize(ConnectionID conn_id, vector<vector<char>>* bufs) override
        {
			json data;
			if (!net_status[conn_id].obj_up_to_date)
			{
				data = obj->serialize(bufs);
				if (node->getParent())
				{
					if (!node->getParent()->getName().empty())
					{
						data["pose_parent"] = node->getParent()->getName();
					}
				}
			}
			if (!net_status[conn_id].pose_up_to_date)
			{
				data["pose"] = toJson(node->getTransform());
			}
            return data;
        }
		
    private:
        shared_ptr<T> obj;
    };

	WindowID id;
	float width;
    float height;
    float x_position; ///< Position of the upper-left window corner along the x-axis.
    float y_position; ///< Position of the upper-left window corner along the y-axis.
	bool perunit_layout = false;
	string title;
	float fps = 30.0f;
	float opacity;
	int frame_messages_waiting = 0;
	int frame_skip_message_limit = 3;
	int frame_message_skips = 0;
	shared_ptr<TransformNode> window_top_right_node;
	shared_ptr<TransformNode> window_bottom_right_node;
	shared_ptr<TransformNode> window_top_left_node;
	shared_ptr<TransformNode> window_bottom_left_node;
	vector<shared_ptr<RenderNode>> objects;
	inline static WindowManager manager;
	std::thread server_message_thread;
	bool shutdown = false;
	json camera_control;
	unordered_map<ConnectionID, bool> camera_up_to_date;
	unordered_map<ConnectionID, int> recorded_messages_written;
	KeyActionCallback key_press_callback = NULL;
	MouseClickActionCallback mouse_click_callback = NULL;
	size_t total_network_use_bytes = 0;
	bool network_use_start_time_set = false;
	std::chrono::time_point<std::chrono::high_resolution_clock> network_use_start_time;
	vector<JSONBMessage> recorded_messages_to_write;
	float last_record_time = 0.0;
	inline static float default_opacity = 1.0f;
	inline static unique_ptr<Process> server_proc;
	inline static string record_path;
	inline static std::chrono::time_point<std::chrono::high_resolution_clock> record_start_time;
};

}