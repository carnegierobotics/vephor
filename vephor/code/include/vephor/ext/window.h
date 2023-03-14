#pragma once

#include "vephor.h"
#include <random>
#include <filesystem>

namespace fs = std::filesystem;

namespace vephor
{
	
// Key codes taken from GLFW
/* The unknown key */
#define KEY_UNKNOWN            -1

/* Printable keys */
#define KEY_SPACE              32
#define KEY_APOSTROPHE         39  /* ' */
#define KEY_COMMA              44  /* , */
#define KEY_MINUS              45  /* - */
#define KEY_PERIOD             46  /* . */
#define KEY_SLASH              47  /* / */
#define KEY_0                  48
#define KEY_1                  49
#define KEY_2                  50
#define KEY_3                  51
#define KEY_4                  52
#define KEY_5                  53
#define KEY_6                  54
#define KEY_7                  55
#define KEY_8                  56
#define KEY_9                  57
#define KEY_SEMICOLON          59  /* ; */
#define KEY_EQUAL              61  /* = */
#define KEY_A                  65
#define KEY_B                  66
#define KEY_C                  67
#define KEY_D                  68
#define KEY_E                  69
#define KEY_F                  70
#define KEY_G                  71
#define KEY_H                  72
#define KEY_I                  73
#define KEY_J                  74
#define KEY_K                  75
#define KEY_L                  76
#define KEY_M                  77
#define KEY_N                  78
#define KEY_O                  79
#define KEY_P                  80
#define KEY_Q                  81
#define KEY_R                  82
#define KEY_S                  83
#define KEY_T                  84
#define KEY_U                  85
#define KEY_V                  86
#define KEY_W                  87
#define KEY_X                  88
#define KEY_Y                  89
#define KEY_Z                  90
#define KEY_LEFT_BRACKET       91  /* [ */
#define KEY_BACKSLASH          92  /* \ */
#define KEY_RIGHT_BRACKET      93  /* ] */
#define KEY_GRAVE_ACCENT       96  /* ` */
#define KEY_WORLD_1            161 /* non-US #1 */
#define KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define KEY_ESCAPE             256
#define KEY_ENTER              257
#define KEY_TAB                258
#define KEY_BACKSPACE          259
#define KEY_INSERT             260
#define KEY_DELETE             261
#define KEY_RIGHT              262
#define KEY_LEFT               263
#define KEY_DOWN               264
#define KEY_UP                 265
#define KEY_PAGE_UP            266
#define KEY_PAGE_DOWN          267
#define KEY_HOME               268
#define KEY_END                269
#define KEY_CAPS_LOCK          280
#define KEY_SCROLL_LOCK        281
#define KEY_NUM_LOCK           282
#define KEY_PRINT_SCREEN       283
#define KEY_PAUSE              284
#define KEY_F1                 290
#define KEY_F2                 291
#define KEY_F3                 292
#define KEY_F4                 293
#define KEY_F5                 294
#define KEY_F6                 295
#define KEY_F7                 296
#define KEY_F8                 297
#define KEY_F9                 298
#define KEY_F10                299
#define KEY_F11                300
#define KEY_F12                301
#define KEY_F13                302
#define KEY_F14                303
#define KEY_F15                304
#define KEY_F16                305
#define KEY_F17                306
#define KEY_F18                307
#define KEY_F19                308
#define KEY_F20                309
#define KEY_F21                310
#define KEY_F22                311
#define KEY_F23                312
#define KEY_F24                313
#define KEY_F25                314
#define KEY_KP_0               320
#define KEY_KP_1               321
#define KEY_KP_2               322
#define KEY_KP_3               323
#define KEY_KP_4               324
#define KEY_KP_5               325
#define KEY_KP_6               326
#define KEY_KP_7               327
#define KEY_KP_8               328
#define KEY_KP_9               329
#define KEY_KP_DECIMAL         330
#define KEY_KP_DIVIDE          331
#define KEY_KP_MULTIPLY        332
#define KEY_KP_SUBTRACT        333
#define KEY_KP_ADD             334
#define KEY_KP_ENTER           335
#define KEY_KP_EQUAL           336
#define KEY_LEFT_SHIFT         340
#define KEY_LEFT_CONTROL       341
#define KEY_LEFT_ALT           342
#define KEY_LEFT_SUPER         343
#define KEY_RIGHT_SHIFT        344
#define KEY_RIGHT_CONTROL      345
#define KEY_RIGHT_ALT          346
#define KEY_RIGHT_SUPER        347
#define KEY_MENU               348
	
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

class Window;
using ObjectID = int;

struct RenderNode
{
public:
	friend Window;

	RenderNode(const shared_ptr<TransformNode>& p_node):node(p_node){}
	virtual ~RenderNode(){}
	virtual json serialize(ConnectionID conn_id, vector<vector<char>>* bufs) = 0;
	Vec3 getPos() const
	{
		return node->getPos();
	}
	void setPos(const Vec3& pos)
	{
		node->setPos(pos);
	}
	Orient3 getOrient() const
	{
		return node->getOrient();
	}
	void setOrient(const Orient3& orient)
	{
		node->setOrient(orient);
	}
	TransformSim3 getTransform() const
	{
		return node->getTransform();
	}
	void setTransform(const TransformSim3& t)
	{
		node->setTransform(t);
	}
	float getScale() const
	{
		return node->getScale();
	}
	void setScale(float scale)
	{
		node->setScale(scale);
	}
	void setParent(TransformNode& parent)
	{
		if (node->getParent() != NULL)
			throw std::runtime_error("RenderNode already has a parent.");
		parent.addChild(node);
	}
	void setParent(RenderNode& parent)
	{
		if (node->getParent() != NULL)
			throw std::runtime_error("RenderNode already has a parent.");
		parent.node->addChild(node);
	}
	void setParent(const shared_ptr<TransformNode>& parent)
	{
		setParent(*parent.get());
	}
	void setParent(const shared_ptr<RenderNode>& parent)
	{
		setParent(*parent.get());
	}
	void setShow(bool p_show)
	{
		show = p_show;
		for (auto& status : net_status)
		{
			status.second.show_up_to_date = false;
		}
	}
	bool isShow() const {return show;}
	void setDestroy() {destroy = true;}
	bool checkDestroy() const {return destroy;}	
	ObjectID getID() const {return id;}
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
	};
	
	unordered_map<ConnectionID, RenderObjectNetworkStatus> net_status;
};

using WindowID = int;
using ObjectID = int;

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
	
	void checkIncomingMessages()
	{
		// Sort through all incoming messages and distribute them to windows
		for (auto id : net.getConnectionIdList())
		{
			for (const auto& msg : net.getIncomingJSONBMessages(id))
			{
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
};


	
class Window
{
public:
    Window(int p_width=-1, int p_height=-1, string p_title="show")
	: width(p_width), height(p_height), title(p_title)
	{
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
	
	~Window()
	{
	}
	
	void setTitle(const string& p_title)
	{
		title = p_title;
		if (camera_control["type"] == "plot")
		{
			camera_control["title"] = title;
		}
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
	
	void setPlotMode(bool equal = false)
	{
		camera_control = {
			{"type", "plot"},
			{"equal", equal},
		};
	}

	json& getCameraControlInfo()
	{
		return camera_control;
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
		
		node->addTransformCallback([this, inner_obj](const TransformSim3& world_from_node)
		{
			for (auto& status : inner_obj->net_status)
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
		const TransformSim3& parent_from_node,
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
		
		node->addTransformCallback([this, inner_obj](const TransformSim3& world_from_node)
		{
			for (auto& status : inner_obj->net_status)
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

private:
	void writeCurrentMessages(const string& path, bool skip_meta = false)
	{
		json meta_data = {
			{"messages", {}}
		};

		static int msg_index = 0;
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
	}

public:
	bool render(bool wait = true)
	{
		manager.first_render = true;

		if (shutdown)
			return false;
		
		if (manager.mode == WindowManager::Mode::Record)	
		{
			JSONBMessage msg;
			json scene = produceSceneJSON(std::numeric_limits<ConnectionID>::max(), &msg.payloads);
			auto end_time = std::chrono::high_resolution_clock::now();
			float record_time = std::chrono::duration<float, std::milli>(end_time-record_start_time).count() / 1000.0f;

			msg.header = {
				{"type", "scene"},
				{"data", scene}
			};
			msg.header["time"] = record_time;
			msg.valid = true;
			messages_to_write.push_back(msg);

			writeCurrentMessages(record_path, true);

			messages_to_write.clear();

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

				manager.net.sendJSONBMessage(conn_id, msg.header, msg.payloads);

				// Record after sending so time doesn't go out in the network message
				if (!record_path.empty())
				{
					auto end_time = std::chrono::high_resolution_clock::now();
					float record_time = std::chrono::duration<float, std::milli>(end_time-record_start_time).count() / 1000.0f;

					msg.header["time"] = record_time;

					messages_to_write.push_back(msg);
					writeCurrentMessages(record_path, true);
					messages_to_write.clear();

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

				

				if (wait)
				{
					json show = {
						{"type", "wait"}
					};
					manager.net.sendJSONBMessage(conn_id, show, {});
				}
			}
			
			// Handle incoming messages
			if (wait)
				v4print "Waiting for close.";
			while (true)
			{
				bool key_pressed = false;
				
				auto msgs = manager.getWindowMessages(id);
				for (const auto& msg : msgs)
				{
					if (msg["type"] == "key_press")
					{
						v4print "Key press:", msg["key"], "on window:", id;
					}
					else if (msg["type"] == "close")
					{
						v4print "Close message received for window:", id;
						shutdown = true;
					}
				}
				
				if (!wait || shutdown)
					break;

				std::this_thread::sleep_for(std::chrono::milliseconds(100));

				json heartbeat = {
					{"type", "heartbeat"}
				};
				if (id > 0)
					heartbeat["window_id"] = id;
				for (auto conn_id : manager.net.getConnectionIdList())
				{
					manager.net.sendJSONBMessage(conn_id, heartbeat, {});
				}
			}
		}
		else
		{
			string temp_dir = getTempDir();
			
			JSONBMessage msg;
			json scene = produceSceneJSON(std::numeric_limits<ConnectionID>::max(), &msg.payloads);
			msg.header = {
				{"type", "scene"},
				{"data", scene}
			};
			msg.valid = true;
			messages_to_write.push_back(msg);

			if (wait)
			{
				writeCurrentMessages(temp_dir);

				v4print "Calling show...";
				Process proc({"vephor_show", "-i", temp_dir, "-r"});
				int result = proc.join();
				if (result != 0)
					v4print "Show exited without success.";
				else
					v4print "Show exited with success.";

				messages_to_write.clear();
				
				fs::remove_all(temp_dir);
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
			if (obj->destroy)
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
		json scene = produceSceneJSON(std::numeric_limits<ConnectionID>::max(), &msg.payloads);
		msg.header = {
			{"type", "scene"},
			{"data", scene}
		};
		msg.valid = true;
		messages_to_write.push_back(msg);
		
		writeCurrentMessages(path);

		if (fs::exists(temp_dir+"/scene_assets"))
		{
			fs::copy(temp_dir+"/scene_assets", path+"/scene_assets");
		}

		messages_to_write.clear();

		fs::remove_all(temp_dir);
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
		const string p_record_path = "")
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
	
		v4print "Starting server process...";
		server_proc = make_unique<Process>(vector<string>{"vephor_show", 
			"-m", "client", 
			"-o", "localhost", 
			"-p", std::to_string(port),
			"-r"});
		v4print "Server process started.";
		
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
				{"title", title}
			};
			scene["camera"]["control"] = camera_control;
			camera_up_to_date[conn_id] = true;
		}
		
		scene["objects"] = json();
		
		for (const auto& obj : objects)
		{
			if (
				obj->net_status[conn_id].obj_up_to_date && 
				obj->net_status[conn_id].pose_up_to_date && 
				obj->net_status[conn_id].show_up_to_date && 
				!obj->destroy)
				continue;
			
			json datum = obj->serialize(conn_id, bufs);
			datum["id"] = obj->id;
			
			if (!obj->net_status[conn_id].show_up_to_date)
			{
				datum["show"] = obj->show;
			}
			
			if (obj->destroy)
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
		}

		return scene;
	}

	struct NullRenderNode : public RenderNode
    {
    public:
        NullRenderNode(const shared_ptr<TransformNode>& p_node)
        : RenderNode(p_node)
        {}

        virtual json serialize(ConnectionID conn_id, vector<vector<char>>* bufs) override
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

        virtual json serialize(ConnectionID conn_id, vector<vector<char>>* bufs) override
        {
			json data;
			if (!net_status[conn_id].obj_up_to_date)
			{
				data = obj->serialize(bufs);
				if (node->getParent())
				{
					v4print id, "has parent", node->getParent()->getName();
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
	int width, height;
	string title;
	shared_ptr<TransformNode> window_top_right_node;
	shared_ptr<TransformNode> window_bottom_right_node;
	shared_ptr<TransformNode> window_top_left_node;
	shared_ptr<TransformNode> window_bottom_left_node;
	vector<shared_ptr<RenderNode>> objects;
	inline static WindowManager manager;
	unordered_map<ConnectionID, bool> camera_up_to_date;
	bool key_pressed = false;
	std::thread server_message_thread;
	bool shutdown = false;
	json camera_control;
	inline static vector<JSONBMessage> messages_to_write;
	inline static unique_ptr<Process> server_proc;
	inline static string record_path;
	inline static std::chrono::time_point<std::chrono::high_resolution_clock> record_start_time;
};

}