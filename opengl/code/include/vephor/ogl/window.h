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

#include "io.h"

namespace vephor
{

class Window;
class Material;

using MouseActionCallback = std::function<void()>;
using ScrollCallback = std::function<void(float)>;
using KeyActionCallback = std::function<void(int)>;
using WindowResizeCallback = std::function<void(Window*, const Vec2i&)>;

void createOpenGLBufferForMatX(GLuint& buffer_id, const MatX& mat);
void addOpenGLBufferToActiveVAO(
    GLuint buffer_id,
    GLuint attr_loc,
    int size
);

// Compile and create shader object and returns its id
GLuint compileShaders(string shader, GLenum type);

// Creates a program containing vertex and fragment shader
// links it and returns its ID
GLuint linkProgram(GLuint vertexShaderId, GLuint fragmentShaderId);

//TODO: I hate this
void global_mouse_handler(GLFWwindow* window, int button, int action, int mods);
void global_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

GLuint findProgram(const string& program_name);
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

struct MonitorInfo
{
    Vec2i size;
    Vec2i pos;
};

struct ShadowOptions
{
    int map_size = 2048;
    bool debug = false;
    float rad_m = 600;
    float border_m = 10;
	float light_height = 100;
};

class Window
{
public:
	//TODO: I hate this
	friend void global_mouse_handler(GLFWwindow* window, int button, int action, int mods);
    friend void global_key_handler(GLFWwindow* window, int key, int scancode, int action, int mods);
	friend void global_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    static const size_t NUM_LAYERS = 20;

    struct LightInfo
    {
        Vec3 pos = Vec3::Zero();
        float strength = 0.0f;
    };

    Window(int width = -1,
           int height = -1,
           const string &title = "show",
           WindowResizeCallback resize_callback = nullptr,
           const WindowOptions &opts = WindowOptions());

    Window(int width,
           int height,
           int x_position,
           int y_position,
           const string &title = "show",
           WindowResizeCallback resize_callback = nullptr,
           const WindowOptions &options = WindowOptions());

    ~Window();

    static std::vector<MonitorInfo> getMonitorInfo();

    void show()
    {
        if (!showing)
        {
            glfwShowWindow(window);
            glfwSetWindowOpacity(window, curr_opacity);
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

    void setTitle(const string& p_title)
    {
        title = p_title;
        glfwSetWindowTitle(window, title.c_str());
    }

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

    void setOpacity(float opacity)
    {
        glfwSetWindowOpacity(window, opacity);
        curr_opacity = glfwGetWindowOpacity(window);
    }

    float getOpacity() const
    {
        return curr_opacity;
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
		
		layer += NUM_LAYERS / 2;

		if (layer < 0)
			layer = 0;
        else if (layer >= NUM_LAYERS)
            layer = NUM_LAYERS - 1;

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

    void setDirLight(const Vec3& dir, float strength, bool shadows=false, const ShadowOptions& opts=ShadowOptions())
    {
		if (window == NULL)
			return;
		
        dir_light.pos = -dir;
        dir_light.strength = strength;
        dir_light_shadows = shadows;
        dir_light_shadow_opts = opts;

        if (!dir_light_shadows)
            return;

        if (opts.map_size == 0)
        {
            throw std::runtime_error("Must have a shadow map size larger than 0.");
        }

        glGenFramebuffers(1, &dir_light_shadow_map_fbo);

        GLuint dir_light_shadow_map_id;
        
        if (opts.map_size != dir_light_shadow_map_size)
        {
            if (dir_light_shadow_map_size > 0)
                glDeleteTextures(1, &dir_light_shadow_map_id);

            glGenTextures(1, &dir_light_shadow_map_id);
            glBindTexture(GL_TEXTURE_2D, dir_light_shadow_map_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                        opts.map_size, opts.map_size, 0, 
                        GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

            dir_light_shadow_map_size = opts.map_size;
        }

        glBindTexture(GL_TEXTURE_2D, dir_light_shadow_map_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float border_color[] = {1.0, 1.0, 1.0, 1.0};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

        glBindFramebuffer(GL_FRAMEBUFFER, dir_light_shadow_map_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dir_light_shadow_map_id, 0);
        glDrawBuffer(GL_NONE); // no color buffer is drawn to
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        dir_light_shadow_map = make_shared<Texture>(dir_light_shadow_map_id, Vec2i(dir_light_shadow_map_size,dir_light_shadow_map_size));
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
        if (shadow_phase)
            return dir_light_proj_matrix;
        if (overlay_phase)
            return overlay_proj_matrix;
        return proj_matrix;
    }
    const Mat4& getCamFromWorldMatrix() const
    {
        if (shadow_phase)
            return dir_light_cam_from_world_matrix;
        if (reflection_phase)
            return reflect_cam_from_world_matrix;
        if (overlay_phase)
            return overlay_cam_from_world_matrix;
        return cam_from_world_matrix;
    }

    const Vec2i& getSize() const {return window_size;}
    const int getWidth() const {return window_size[0];}
    const int getHeight() const {return window_size[1];}

    [[nodiscard]] const Vec2i& getPosition() const { return window_position; }
    [[nodiscard]] const int getXPosition() const { return window_position[0]; }
    [[nodiscard]] const int getYPosition() const { return window_position[1]; }

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

    void onResize(const Vec2i &window_size)
    {
        if (window_top_left_node != nullptr)
        {
            window_top_left_node->setPos({0.0F, static_cast<float>(window_size[1]), 0.0F});
        }
        if (window_top_node != nullptr)
        {
            window_top_node->setPos(
                {static_cast<float>(window_size[0]) / 2.0F, static_cast<float>(window_size[1]), 0.0F});
        }
        if (window_top_right_node != nullptr)
        {
            window_top_right_node->setPos(
                {static_cast<float>(window_size[0]), static_cast<float>(window_size[1]), 0.0F});
        }
        if (window_left_node != nullptr)
        {
            window_left_node->setPos({0.0F, static_cast<float>(window_size[1]) / 2.0F, 0.0F});
        }
        if (window_center_node != nullptr)
        {
            window_center_node->setPos(
                {static_cast<float>(window_size[0]) / 2.0F, static_cast<float>(window_size[1]) / 2.0F, 0.0F});
        }
        if (window_right_node != nullptr)
        {
            window_right_node->setPos(
                {static_cast<float>(window_size[0]), static_cast<float>(window_size[1]) / 2.0F, 0.0F});
        }
        if (window_bottom_left_node != nullptr)
        {
            window_bottom_left_node->setPos({0.0F, 0.0F, 0.0F});
        }
        if (window_bottom_node != nullptr)
        {
            window_bottom_node->setPos({static_cast<float>(window_size[0]) / 2.0F, 0.0F, 0.0F});
        }
        if (window_bottom_right_node != nullptr)
        {
            window_bottom_right_node->setPos({static_cast<float>(window_size[0]), 0.0F, 0.0F});
        }

        resize_callback(this, window_size);
    }

	Vec2 getContentScale() const {return content_scale;}

    shared_ptr<TransformNode> getWindowTopLeftNode() const { return window_top_left_node; }
	shared_ptr<TransformNode> getWindowTopNode() const { return window_top_node; }
    shared_ptr<TransformNode> getWindowTopRightNode() const { return window_top_right_node; }
	shared_ptr<TransformNode> getWindowLeftNode() const { return window_left_node; }
    shared_ptr<TransformNode> getWindowCenterNode() const { return window_center_node; }
	shared_ptr<TransformNode> getWindowRightNode() const { return window_right_node; }
    shared_ptr<TransformNode> getWindowBottomLeftNode() const { return window_bottom_left_node; }
	shared_ptr<TransformNode> getWindowBottomNode() const { return window_bottom_node; }
    shared_ptr<TransformNode> getWindowBottomRightNode() const { return window_bottom_right_node; }

	void shutdown();

    json produceSceneJSON(vector<vector<char>>* bufs = NULL);
    void saveSceneArtifacts(const string& save_dir);

    shared_ptr<Texture> loadTexture(const std::string& path, bool nearest = false);
    shared_ptr<Texture> getTextureFromBuffer(const char* buf_data, int buf_size, const Vec2i& size, bool nearest = false);
    shared_ptr<Texture> getTextureFromImage(const Image<uint8_t>& img, bool nearest = false);

    shared_ptr<CubeTexture> getCubeTextureFromDir(const string& faces_dir, bool nearest = false);
    shared_ptr<CubeTexture> getCubeTextureFromImages(const vector<string>& faces, bool nearest = false);

    using TexturePathResolveCallback = std::function<string(const string&)>;
    shared_ptr<Texture> getTextureFromJSON(const json& data, int base_buf_index, const vector<vector<char>>& bufs, TexturePathResolveCallback path_callback = NULL, json* serial_header = NULL, vector<vector<char>>* serial_bufs = NULL);

    Image<uint8_t> getScreenImage();
    Image<uint8_t> getDepthImage();
    
    void clear();

    static void setDebug(bool p_debug = true)
	{
		debug = p_debug;
	}

    void setCanonicalTime(float time)
    {
        canonical_time = time;
    }
    float getCanonicalTime() const
    {
        return canonical_time;
    }

    void setNearZ(float p_near_z)
    {
        near_z = p_near_z;
    }
    float getNearZ() const
    {
        return near_z;
    }
    void setFarZ(float p_far_z)
    {
        far_z = p_far_z;
    }
    float getFarZ() const
    {
        return far_z;
    }

    GLuint getActiveReflectiveTexture() const {return reflect_texture;}
    bool isReflectionPhase() const {return reflection_phase;}
    bool isShadowPhase() const {return shadow_phase;}
    bool areShadowsActive() const {return dir_light_shadows;}

    struct ReflectiveSurfaceTextures
    {
        shared_ptr<Texture> tex;
        shared_ptr<Texture> depth;
    };

    ReflectiveSurfaceTextures addReflectiveSurface(const Vec4& plane, bool keep_depth=false);

    bool getDirLightShadowInfo(Mat4& out_dir_light_proj_from_world, shared_ptr<Texture>& out_dir_light_shadow_map)
    {
        if (!dir_light_shadows)
            return false;
        
        out_dir_light_proj_from_world = dir_light_proj_matrix * dir_light_cam_from_world_matrix;
        out_dir_light_shadow_map = dir_light_shadow_map;
        return true;
    }

    void setResizeCallback(const WindowResizeCallback p_resize_callback)
    {
        resize_callback = p_resize_callback;
    }
    void triggerResize()
    {
        onResize(window_size);
    }
private:
    void removeDestroyedObjects(vector<shared_ptr<RenderNode>>& objects);
    void renderDirLightShadowMap();
    void renderScene();

    Image<uint8_t> getFBOImage(GLuint fbo, int width, int height);
    Image<uint8_t> getFBODepthImage(GLuint fbo, int width, int height);

    struct ReflectiveSurface
    {
        Vec4 plane;
        Mat4 reflection;
        GLuint fbo;
        GLuint texture;
        GLuint depth_buffer;
        int width;
        int height;
    };

    void renderReflectiveSurface(ReflectiveSurface& surface);

    vector<ReflectiveSurface> reflective_surfaces;

    WindowResizeCallback resize_callback;
    Vec2i window_size;
    Vec2i window_position; ///< Position of the window's upper-left corner.
    string title;
    float near_z = 0.1f;
    float far_z = 1000.0f;
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
    bool reflection_phase = false;
    bool shadow_phase = false;
    Mat4 reflect_cam_from_world_matrix;
    GLuint reflect_texture = std::numeric_limits<GLuint>::max();

    shared_ptr<Texture> default_tex;
	shared_ptr<Texture> default_normal_map;
    unordered_map<int, LightInfo> point_lights;
    LightInfo dir_light;
    bool dir_light_shadows = false;
    ShadowOptions dir_light_shadow_opts;
    int dir_light_shadow_map_size = 0;
    shared_ptr<Texture> dir_light_shadow_map;
    GLuint dir_light_shadow_map_fbo = std::numeric_limits<GLuint>::max();
    Mat4 dir_light_proj_matrix;
    Mat4 dir_light_cam_from_world_matrix;
    shared_ptr<Material> depth_only_material;
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

	shared_ptr<TransformNode> window_top_left_node;
	shared_ptr<TransformNode> window_top_node;
	shared_ptr<TransformNode> window_top_right_node;
	shared_ptr<TransformNode> window_left_node;
    shared_ptr<TransformNode> window_center_node;
	shared_ptr<TransformNode> window_right_node;
	shared_ptr<TransformNode> window_bottom_left_node;
	shared_ptr<TransformNode> window_bottom_node;
	shared_ptr<TransformNode> window_bottom_right_node;

    vector<vector<shared_ptr<RenderNode>>> object_layers;
	vector<vector<shared_ptr<RenderNode>>> overlay_object_layers;
	vector<shared_ptr<RenderNode>> non_render_objects;
    bool on_top = false;
    float curr_opacity = 1.0f;
    float canonical_time = 0.0f;

    inline static bool debug = false;
};

}