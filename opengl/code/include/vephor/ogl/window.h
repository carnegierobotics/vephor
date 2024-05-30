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

#include "io.h"

namespace vephor
{

class Window;

using MouseActionCallback = std::function<void()>;
using ScrollCallback = std::function<void(float)>;
using KeyActionCallback = std::function<void(int)>;
using WindowResizeCallback = std::function<void(Window*, const Vec2i&)>;

// Compile and create shader object and returns its id
GLuint compileShaders(string shader, GLenum type);

// Creates a program containing vertex and fragment shader
// links it and returns its ID
GLuint linkProgram(GLuint vertexShaderId, GLuint fragmentShaderId);

//TODO: I hate this
void global_mouse_handler(GLFWwindow* window, int button, int action, int mods);
void global_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

GLuint buildProgram(const string& program_name, const string& vert_shader, const string& frag_shader);

class Window;

struct RenderNode
{
public:
    struct RenderNodeProperties
    {
        bool show = true;
	    bool destroy = false;
        vector<shared_ptr<RenderNodeProperties>> children;

        void setShow(bool p_show)
        {
            show = p_show;
            for (auto& child : children)
            {
                child->setShow(show);
            }
        }
        void setDestroy()
        {
            destroy = true;
            for (auto& child : children)
            {
                child->setDestroy();
            }
        }
    };

	RenderNode(const shared_ptr<TransformNode>& p_node)
	: node(p_node)
	{
        props = std::make_shared<RenderNodeProperties>();
    }
	virtual ~RenderNode(){}
	virtual void render(Window* window) = 0;
	Vec3 getPos() const
	{
		return node->getPos();
	}
	RenderNode& setPos(const Vec3& pos)
	{
		node->setPos(pos);
		return *this;
	}
	Orient3 getOrient() const
	{
		return node->getOrient();
	}
	RenderNode& setOrient(const Orient3& orient)
	{
		node->setOrient(orient);
		return *this;
	}
	TransformSim3 getTransform() const
	{
		return node->getTransform();
	}
	TransformSim3 getWorldTransform() const
	{
		return node->getWorldTransform();
	}
	RenderNode& setTransform(const TransformSim3& t)
	{
		node->setTransform(t);
		return *this;
	}
	float getScale() const
	{
		return node->getScale();
	}
	RenderNode& setScale(float scale)
	{
		node->setScale(scale);
		return *this;
	}
	RenderNode& setParent(TransformNode& parent)
	{
		if (node->getParent() != NULL)
			throw std::runtime_error("RenderNode already has a parent.");
		parent.addChild(node);
		return *this;
	}
	RenderNode& setParent(RenderNode& parent)
	{
		if (node->getParent() != NULL)
			throw std::runtime_error("RenderNode already has a parent.");
		parent.node->addChild(node);
        parent.props->children.push_back(props);
		return *this;
	}
	RenderNode& setParent(const shared_ptr<TransformNode>& parent)
	{
		return setParent(*parent.get());
	}
	RenderNode& setParent(const shared_ptr<RenderNode>& parent)
	{
		return setParent(*parent.get());
	}
	void setShow(bool p_show){props->setShow(p_show);}
	bool isShow() const {return props->show;}
	void setDestroy() {props->setDestroy();}
	bool checkDestroy() const {return props->destroy;}
    virtual json serialize(vector<vector<char>>* bufs) = 0;
    virtual void saveArtifacts(const string& stub) = 0;
    void setName(const string& name){node->setName(name);}
	void setObjectSerialization(const JSONBMessage& p_obj_ser){obj_serialization = p_obj_ser;}
	const JSONBMessage* getObjectSerialization() const {return &obj_serialization;}
    virtual void onRemoveFromWindow(Window* window){}
protected:
	shared_ptr<TransformNode> node;
    shared_ptr<RenderNodeProperties> props;
private:
	//bool show = true;
	//bool destroy = false;
	JSONBMessage obj_serialization;
};

struct WindowOptions
{
    bool fullscreen = false;
    bool show = true;
    bool always_on_top = false;
};

class Window
{
public:
	//TODO: I hate this
	friend void global_mouse_handler(GLFWwindow* window, int button, int action, int mods);
    friend void global_key_handler(GLFWwindow* window, int key, int scancode, int action, int mods);
	friend void global_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    static const size_t num_layers = 20;

    struct LightInfo
    {
        Vec3 pos = Vec3::Zero();
        float strength = 0.0f;
    };

    Window(
		int width = -1, 
		int height = -1, 
		string title = "show", 
		WindowResizeCallback resize_callback = NULL, 
		const WindowOptions& opts = WindowOptions());
    ~Window();

    void show()
    {
        if (!showing)
        {
            glfwShowWindow(window);
            showing = true;
        }
    }
    void hide()
    {
        if (showing)
        {
            glfwHideWindow(window);
            showing = false;
        }
    }
    bool isShow() const {return showing;}

    bool isAlwaysOnTop() const
    {
        return on_top;
    }

    void toggleAlwaysOnTop()
    {
        on_top = !on_top;
        glfwSetWindowAttrib(window, GLFW_FLOATING, on_top ? GLFW_TRUE : GLFW_FALSE);

        v4print "Setting window to always on top:", on_top;
    }

    void setHideOnClose(bool p_hide_on_close)
    {
        hide_on_close = p_hide_on_close;
    }

    void setIgnoreClose(bool p_ignore_close)
    {
        ignore_close = p_ignore_close;
    }

    void setFrameLock(float p_fps){fps = p_fps;}

    void setClearColor(const Vec3& color, float alpha = 0.0f)
    {
        glClearColor(color[0], color[1], color[2], alpha);
    }
	
    shared_ptr<RenderNode> add(
		const TransformSim3& parent_from_node = TransformSim3()
	)
    {
		if (window == NULL)
			return NULL;
		
		auto node = make_shared<TransformNode>(parent_from_node);

		shared_ptr<RenderNode> inner_obj = make_shared<NullRenderNode>(node);
		non_render_objects.push_back(inner_obj);
		return inner_obj;
    }
	
    // The object T is a "drawing object", and must:
    //  Implement onAddToWindow, a method which is expected to allocate memory/structures specific to drawing on that window
    //  Implement onRemoveFromWindow, a method which is expected to undo what onAddToWindow did
    //  Implement renderOGL, a method which is expected to cause the object to be drawn to the window
    // Note that to save memory, we allow for objects to be added to a window more than once with different poses, but we do not 
    // require that this be allowed.
    // One example where this is allowed is with mesh objects - meshes involve a considerable memory burden, and so we
    // want to avoid allocating GPU memory for them multiple times if they are being drawn several times in one scene.  Any
    // object that can be added more than once needs to not bind to the pose it is added to the window with.
    // On the other hand, a positional light object binds itself to its pose by nature of the way it is implemented here, and so this
    // object must not allow itself to be added to a window multiple times.  Each object must track its own window addition conditions
    // and throw errors when these conditions are violated.
    // We also allow objects to support multiple windows at once if desired, but no current object supports this explicitly.
	template <class T>
    shared_ptr<RenderNode> add(
		const shared_ptr<T>& obj,
		const TransformSim3& parent_from_node = TransformSim3(),
		bool on_overlay = false, 
		int layer = 0
	)
    {
		if (window == NULL)
			return NULL;

        glfwMakeContextCurrent(window);
		
		layer += num_layers / 2;

		if (layer < 0)
			layer = 0;
        else if (layer >= num_layers)
            layer = num_layers - 1;

		auto node = make_shared<TransformNode>(parent_from_node);

		obj->onAddToWindow(this, node);

		shared_ptr<RenderNode> inner_obj = make_shared<TRenderNode<T>>(obj, node);
		if (on_overlay)
			overlay_object_layers[layer].push_back(inner_obj);
		else
			object_layers[layer].push_back(inner_obj);
		return inner_obj;
    }
	
    int addPointLight(const Vec3& pos, float strength)
    {
		if (window == NULL)
			return -1;
		
        point_lights[light_id] = {pos, strength};
        light_id++;
        return light_id-1;
    }
    void updatePointLight(int light_id, const Vec3& pos, float strength)
    {
		if (window == NULL)
			return;
		
        point_lights[light_id].pos = pos;
        point_lights[light_id].strength = strength;
    }
    void removePointLight(int light_id)
    {
        point_lights.erase(light_id);
    }

    void setDirLight(const Vec3& dir, float strength)
    {
		if (window == NULL)
			return;
		
        dir_light.pos = -dir;
        dir_light.strength = strength;
    }
    const LightInfo& getDirLight() const
    {
        return dir_light;
    }
	
	void setAmbientLight(const Vec3& strength)
	{
		if (window == NULL)
			return;
		
		ambient_light_strength = strength;
	}
	const Vec3& getAmbientLight() const
    {
		if (overlay_phase)
           return overlay_ambient_light_strength;
        return ambient_light_strength;
    }

    const unordered_map<int, LightInfo> getPointLights() const {return point_lights;}

    void setCamFromWorld(const Transform3& p_cam_from_world)
    {
		if (window == NULL)
			return;
		
        cam_from_world = p_cam_from_world;
        cam_from_world_matrix = cam_from_world.matrix();
    }
    void rotateCamera(const Vec3& cam_from_new_cam)
    {
		if (window == NULL)
			return;
		
        cam_from_world = Transform3(Vec3::Zero(), cam_from_new_cam).inverse() * cam_from_world;
        cam_from_world_matrix = cam_from_world.matrix();
    }
    const Transform3& getCamFromWorld() const
    {
        // if (overlay_phase)
        //    return overlay_cam_from_world;
        return cam_from_world;
    }

    void setGLFromWorld(const Mat4& p_gl_from_world)
    {
		if (window == NULL)
			return;
		
        gl_from_world = p_gl_from_world;
        proj_matrix = pure_proj_matrix * gl_from_world;
    }
    void setProjectionMatrix(const Mat4& p_proj_matrix)
    {
		if (window == NULL)
			return;
		
        pure_proj_matrix = p_proj_matrix.transpose();
        proj_matrix = pure_proj_matrix * gl_from_world;
    }
    void setOverlayProjectionMatrix(const Mat4& p_proj_matrix)
    {
		if (window == NULL)
			return;
		
        overlay_proj_matrix = p_proj_matrix.transpose();
    }
    const Mat4& getProjectionMatrix() const
    {
        if (overlay_phase)
            return overlay_proj_matrix;
        return proj_matrix;
    }
    const Mat4& getCamFromWorldMatrix() const
    {
        if (overlay_phase)
            return overlay_cam_from_world_matrix;
        return cam_from_world_matrix;
    }

    const Vec2i& getSize() const {return window_size;}
    const int getWidth() const {return window_size[0];}
    const int getHeight() const {return window_size[1];}

    const shared_ptr<Texture> getDefaultTex() const {return default_tex;}
	const shared_ptr<Texture> getDefaultNormalMap() const {return default_normal_map;}

    bool render();

    Vec2 getMousePos() const
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return Vec2(x,window_size[1]-y);
    }
    void setLeftMouseButtonPressCallback(MouseActionCallback p_left_press_callback)
    {
        left_press_callback = p_left_press_callback;
    }
    void setLeftMouseButtonReleaseCallback(MouseActionCallback p_left_release_callback)
    {
        left_release_callback = p_left_release_callback;
    }
    void setRightMouseButtonPressCallback(MouseActionCallback p_right_press_callback)
    {
        right_press_callback = p_right_press_callback;
    }
    void setRightMouseButtonReleaseCallback(MouseActionCallback p_right_release_callback)
    {
        right_release_callback = p_right_release_callback;
    }
    void setKeyPressCallback(KeyActionCallback p_key_callback)
    {
        key_press_callback = p_key_callback;
    }
    void setKeyReleaseCallback(KeyActionCallback p_key_callback)
    {
        key_release_callback = p_key_callback;
    }
    void setScrollCallback(ScrollCallback p_scroll_callback)
    {
        scroll_callback = p_scroll_callback;
    }
	
	void getWorldRayForMousePos(const Vec2& mpos, Vec3& origin, Vec3& ray);
	
	void setWindowSize(const Vec2i& p_window_size){window_size = p_window_size;}

    void onResize(const Vec2i& window_size)
	{
		if (window_top_right_node)
			window_top_right_node->setPos(Vec3(window_size[0], window_size[1], 0));
		
		if (window_bottom_right_node)
			window_bottom_right_node->setPos(Vec3(window_size[0], 0, 0));
		
		if (window_top_left_node)
			window_top_left_node->setPos(Vec3(0, window_size[1], 0));
		
		resize_callback(this, window_size);
	}
	
	Vec2 getContentScale() const {return content_scale;}
	
	shared_ptr<TransformNode> getWindowTopRightNode() const {return window_top_right_node;}
	shared_ptr<TransformNode> getWindowBottomRightNode() const {return window_bottom_right_node;}
	shared_ptr<TransformNode> getWindowTopLeftNode() const {return window_top_left_node;}
	shared_ptr<TransformNode> getWindowBottomLeftNode() const {return window_bottom_left_node;}
	
	void shutdown();

    json produceSceneJSON(vector<vector<char>>* bufs = NULL);
    void saveSceneArtifacts(const string& save_dir);

    shared_ptr<Texture> loadTexture(const std::string& path, bool nearest = false);
    shared_ptr<Texture> getTextureFromBuffer(const char* buf_data, int buf_size, const Vec2i& size, bool nearest = false);
    shared_ptr<Texture> getTextureFromImage(const Image<uint8_t>& img, bool nearest = false);

    using TexturePathResolveCallback = std::function<string(const string&)>;
    shared_ptr<Texture> getTextureFromJSON(const json& data, int base_buf_index, const vector<vector<char>>& bufs, TexturePathResolveCallback path_callback = NULL, json* serial_header = NULL, vector<vector<char>>* serial_bufs = NULL);

    Image<uint8_t> getScreenImage();
    Image<uint8_t> getDepthImage();
private:
    void removeDestroyedObjects(vector<shared_ptr<RenderNode>>& objects);

    WindowResizeCallback resize_callback;
    Vec2i window_size;
    string title;
	Vec2 content_scale = Vec2(1,1);
    GLFWwindow* window = NULL;
    int id;
    bool showing = true;
    bool ignore_close = false;
    bool hide_on_close = false;
    inline static int next_id = 0;
    Mat4 proj_matrix;
    Transform3 cam_from_world;
    Mat4 overlay_proj_matrix;
    Mat4 overlay_cam_from_world_matrix = Mat4::Identity();
    Mat4 cam_from_world_matrix;
    Mat4 pure_proj_matrix;
    Mat4 gl_from_world;

    MouseActionCallback left_press_callback = NULL;
    MouseActionCallback left_release_callback = NULL;
    MouseActionCallback right_press_callback = NULL;
    MouseActionCallback right_release_callback = NULL;
    KeyActionCallback key_press_callback = NULL;
    KeyActionCallback key_release_callback = NULL;
	ScrollCallback scroll_callback = NULL;
    bool escape_pressed = false;

    bool overlay_phase = false;

    shared_ptr<Texture> default_tex;
	shared_ptr<Texture> default_normal_map;
    unordered_map<int, LightInfo> point_lights;
    LightInfo dir_light;
	Vec3 overlay_ambient_light_strength = Vec3(1,1,1);
	Vec3 ambient_light_strength;
    int light_id = 0;

    float fps = 0;
    time_point<high_resolution_clock> last_time;

	struct NullRenderNode : public RenderNode
    {
    public:
        NullRenderNode(const shared_ptr<TransformNode>& p_node)
        : RenderNode(p_node)
        {}

        virtual void render(Window* window) override{}
        virtual json serialize(vector<vector<char>>* bufs) override
        {
            if (node->getName().empty())
                return json();

			auto serialization = getObjectSerialization();
			if (!serialization->valid)
				return json();
			if (!serialization->payloads.empty() && !bufs)
				return json();
			
			json data = serialization->header;
            data["id"] = std::stoll(node->getName());
			
			if (bufs)
			{
				data["base_buf_index"] = bufs->size();
				extend(*bufs, serialization->payloads);
			}
			
            if (node->getParent())
            {
                if (!node->getParent()->getName().empty())
                {
                    data["pose_parent"] = node->getParent()->getName();
                }
            }
            data["pose"] = toJson(node->getTransform());
            return data;
        }
        void saveArtifacts(const string& stub) override
        {
        }
    };

    template <class T>
    struct TRenderNode : public RenderNode
    {
    public:
        TRenderNode(const shared_ptr<T>& p_obj, const shared_ptr<TransformNode>& p_node)
        : RenderNode(p_node), obj(p_obj)
        {}

        virtual void render(Window* window) override
        {
            obj->renderOGL(window, node->getWorldTransform());
        }
        virtual json serialize(vector<vector<char>>* bufs) override
        {
            if (node->getName().empty())
                return json();

            auto serialization = getObjectSerialization();
			if (!serialization->valid)
				return json();
			if (!serialization->payloads.empty() && !bufs)
				return json();
			
			json data = serialization->header;
            data["id"] = std::stoll(node->getName());
			
			if (bufs)
			{
				data["base_buf_index"] = bufs->size();
				extend(*bufs, serialization->payloads);
			}
			
            if (node->getParent())
            {
                v4print "object has parent", node->getParent()->getName();
                if (!node->getParent()->getName().empty())
                {
                    data["pose_parent"] = node->getParent()->getName();
                }
            }
            data["pose"] = toJson(node->getTransform());
            return data;
        }
        void saveArtifacts(const string& stub) override
        {
            //obj->saveArtifacts(stub);
        }
        virtual void onRemoveFromWindow(Window* window) override
        {
            obj->onRemoveFromWindow(window);
        }
    private:
        shared_ptr<T> obj;
    };

	shared_ptr<TransformNode> window_top_right_node;
	shared_ptr<TransformNode> window_bottom_right_node;
	shared_ptr<TransformNode> window_top_left_node;
	shared_ptr<TransformNode> window_bottom_left_node;
    vector<vector<shared_ptr<RenderNode>>> object_layers;
	vector<vector<shared_ptr<RenderNode>>> overlay_object_layers;
	vector<shared_ptr<RenderNode>> non_render_objects;
    bool on_top = false;
};

}