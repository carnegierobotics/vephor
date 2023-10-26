#include "vephor/ogl/window.h"

namespace vephor
{

// Compile and create shader object and returns its id
GLuint compileShaders(string shader, GLenum type)
{
    const char *shaderCode = shader.c_str();
    GLuint shaderId = glCreateShader(type);

    if (shaderId == 0)
    { // Error: Cannot create shader object
        cout << "Error creating shaders!";
        return 0;
    }

    // Attach source code to this object
    glShaderSource(shaderId, 1, &shaderCode, NULL);
    glCompileShader(shaderId); // compile the shader object

    GLint compileStatus;

    // check for compilation status
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);

    if (!compileStatus)
    { // If compilation was not successfull
        int length;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        char *cMessage = new char[length];

        // Get additional information
        glGetShaderInfoLog(shaderId, length, &length, cMessage);
        cout << "Cannot Compile Shader: " << cMessage;
        delete[] cMessage;
        glDeleteShader(shaderId);
        return 0;
    }

    return shaderId;
}

// Creates a program containing vertex and fragment shader
// links it and returns its ID
GLuint linkProgram(GLuint vertexShaderId, GLuint fragmentShaderId)
{
    GLuint programId = glCreateProgram(); // crate a program

    if (programId == 0)
    {
        cout << "Error Creating Shader Program";
        return 0;
    }

    // Attach both the shaders to it
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    // Create executable of this program
    glLinkProgram(programId);

    GLint linkStatus;

    // Get the link status for this program
    glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);

    if (!linkStatus)
    { // If the linking failed
        cout << "Error Linking program";
        glDetachShader(programId, vertexShaderId);
        glDetachShader(programId, fragmentShaderId);
        glDeleteProgram(programId);

        return 0;
    }

    return programId;
}

unordered_map<GLFWwindow*, Window*> v4_window_from_gl_window;
GLFWwindow* first_window = NULL;
bool glfw_initialized = false;

void global_mouse_handler(GLFWwindow* gl_window, int button, int action, int mods){
	auto window_ptr = v4_window_from_gl_window[gl_window];
	
	if (window_ptr->left_press_callback && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		window_ptr->left_press_callback();
	
	if (window_ptr->left_release_callback && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		window_ptr->left_release_callback();
	
	if (window_ptr->right_press_callback && button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		window_ptr->right_press_callback();
	
	if (window_ptr->right_release_callback && button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		window_ptr->right_release_callback();
}

void global_key_handler(GLFWwindow* gl_window, int key, int scancode, int action, int mods){
	auto window_ptr = v4_window_from_gl_window[gl_window];
	
	if (window_ptr->key_press_callback && action == GLFW_PRESS)
		window_ptr->key_press_callback(key);
	
	if (window_ptr->key_release_callback && action == GLFW_RELEASE)
		window_ptr->key_release_callback(key);
}

void global_scroll_callback(GLFWwindow* gl_window, double xoffset, double yoffset){	
	auto window_ptr = v4_window_from_gl_window[gl_window];

	if (window_ptr->scroll_callback)
		window_ptr->scroll_callback(yoffset);
}

void global_framebuffer_size_callback(GLFWwindow* gl_window, int width, int height)
{
	glfwMakeContextCurrent(gl_window);
	
	auto window_ptr = v4_window_from_gl_window[gl_window];
	
	Vec2i window_size;
	glfwGetFramebufferSize(gl_window, &window_size[0], &window_size[1]);
	window_ptr->setWindowSize(window_size);
	
	glViewport(0, 0, width, height);

	window_ptr->onResize(Vec2i(width, height));

	window_ptr->setOverlayProjectionMatrix(makeOrthoProj(
        Vec3(0,0,-1), 
        Vec3(width,height,1)
    ));
}

unordered_map<string, GLuint> program_bank;

GLuint buildProgram(const string& program_name, const string& vert_shader, const string& frag_shader)
{
	GLuint program_id;
	if (find(program_bank, program_name))
	{
		program_id = program_bank[program_name];
	}
	else
	{		
		GLuint vert_shader_id;
		GLuint frag_shader_id;	
		vert_shader_id = compileShaders(vert_shader, GL_VERTEX_SHADER);
		frag_shader_id = compileShaders(frag_shader, GL_FRAGMENT_SHADER);
		program_id = linkProgram(vert_shader_id, frag_shader_id);
		program_bank[program_name] = program_id;
	}
	return program_id;
	
	// TODO Later: glDeleteProgram(program_id);
}

Window::Window(int p_width, int p_height, string p_title, WindowResizeCallback p_resize_callback, const WindowOptions& opts)
: resize_callback(p_resize_callback), title(p_title)
{
	id = next_id;
	next_id++;

	if (resize_callback == NULL)
	{
		resize_callback = [](Window* this_window, const Vec2i& window_size){
			Mat4 proj = makePerspectiveProj(45, window_size, 0.1f, 100.0f);
    		this_window->setProjectionMatrix(proj);
		};
	}

    // Initialise GLFW
	if (!glfw_initialized)
	{
		if( !glfwInit() )
		{
			fprintf( stderr, "Failed to initialize GLFW\n" );
			getchar();
			throw std::runtime_error("Failed to initialize GLFW");
		}
		glfw_initialized = true;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (!opts.show)
	{
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		showing = false;
	}

	auto* monitor = glfwGetPrimaryMonitor();
	int mon_xpos, mon_ypos;
	glfwGetMonitorPos(monitor, &mon_xpos, &mon_ypos);
	const auto* mode = glfwGetVideoMode(monitor);

	bool monitor_size = false;
	if (p_width == -1 && p_height == -1)
	{
		p_width = mode->width;
		p_height = mode->height;
		monitor_size = true;
	}
	
	glfwGetMonitorContentScale(monitor, &content_scale[0], &content_scale[1]);

	if (opts.fullscreen)
	{
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

		window = glfwCreateWindow(p_width, p_height, title.c_str(), monitor, first_window);
	}
	else
	{
		window = glfwCreateWindow(p_width, p_height, title.c_str(), NULL, first_window);

		if (!monitor_size)
		{
			int xpos = 50 + (mode->width / 3.0f) * (id % 3);
			int ypos = 50 + 50 * int(id / 3);
			if (xpos + p_width > mode->width)
				xpos = mode->width - p_width;
			if (ypos + p_height > mode->height)
				ypos = mode->height - p_height;
			glfwSetWindowPos(window, mon_xpos + xpos, mon_ypos + ypos);
		}
	}

	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window.\n" );
		getchar();
		glfwTerminate();
		throw std::runtime_error("Failed to open GLFW window.");
	}

	if (opts.always_on_top)
	{
		glfwSetWindowAttrib(window, GLFW_FLOATING, GLFW_TRUE);
	}
	
	if (first_window == NULL)
	{
		first_window = window;
	}
	glfwMakeContextCurrent(window);

	glfwSwapInterval(0);

	glfwGetFramebufferSize(window, &window_size[0], &window_size[1]);

    // Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		throw std::runtime_error("Failed to initialize GLEW");
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.01f);

	gl_from_world = Mat4::Identity();

	pure_proj_matrix = Mat4::Identity();

	proj_matrix = pure_proj_matrix * gl_from_world;
	cam_from_world = Transform3();
	cam_from_world_matrix.setIdentity();

	overlay_proj_matrix = makeOrthoProj(
        Vec3(0,0,-1), 
        Vec3(p_width,p_height,1)
    ).transpose();

	default_tex = getTextureFromImage(*generateSimpleImage(Vec2i(8,8), Vec3(1.0,1.0,1.0)));
	default_normal_map = getTextureFromImage(*generateSimpleImage(Vec2i(8,8), Vec3(0.5,0.5,1.0)));

	last_time = high_resolution_clock::now();

	//TODO: I hate this
	v4_window_from_gl_window[window] = this;

	glfwSetMouseButtonCallback(window, global_mouse_handler);
	glfwSetKeyCallback(window, global_key_handler);
	glfwSetScrollCallback(window, global_scroll_callback);
	glfwSetFramebufferSizeCallback(window, global_framebuffer_size_callback);

	object_layers.resize(num_layers);
	overlay_object_layers.resize(num_layers);

	ambient_light_strength = Vec3(0.1f,0.1f,0.1f);

	window_top_right_node = make_shared<TransformNode>(Transform3());
	window_top_right_node->setName("window_top_right");

	window_bottom_right_node = make_shared<TransformNode>(Transform3());
	window_bottom_right_node->setName("window_bottom_right");

	window_top_left_node = make_shared<TransformNode>(Transform3());
	window_top_left_node->setName("window_top_left");

	window_bottom_left_node = make_shared<TransformNode>(Transform3());
	window_bottom_left_node->setName("window_bottom_left");

	onResize(window_size);
}

Window::~Window()
{
}

void Window::removeDestroyedObjects(vector<shared_ptr<RenderNode>>& objects)
{
	vector<size_t> alive_obj_inds;
	alive_obj_inds.reserve(objects.size());

	for (size_t i = 0; i < objects.size(); i++)
	{
		const auto& obj = objects[i];
		
		if (obj->checkDestroy())
			obj->onRemoveFromWindow(this);
		else
			alive_obj_inds.push_back(i);
	}
	
	if (alive_obj_inds.size() < objects.size())
	{
		vector<shared_ptr<RenderNode>> alive_objects;
		alive_objects.reserve(objects.size());
		
		for (auto ind : alive_obj_inds)
		{
			alive_objects.push_back(objects[ind]);
		}
		
		objects = move(alive_objects);
	}
}

void Window::getWorldRayForMousePos(const Vec2& mpos, Vec3& origin, Vec3& ray)
{
	Vec2 h_window_size = window_size.cast<float>() / 2;
	Vec2 pos = (mpos - h_window_size).array() / h_window_size.array();
	
	Transform3 world_from_cam = cam_from_world.inverse();
	
	origin = world_from_cam.translation();
	ray = (pure_proj_matrix.inverse() * Vec4(pos[0],pos[1],-1,1)).head<3>();
	
	ray /= ray.norm();
	
	ray = world_from_cam.rotation() * ray;
}

bool Window::render()
{
	if (window == NULL)
		return false;
	
	glfwMakeContextCurrent(window);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	overlay_phase = false;

	int objects_rendered = 0;

	for (auto& objects : object_layers)
	{
		removeDestroyedObjects(objects);
		
		vector<size_t> obj_inds;
		obj_inds.reserve(objects.size());
		
		vector<Vec3> cam_pos;
		cam_pos.reserve(objects.size());

		for (const auto& obj : objects)
		{
			obj_inds.push_back(obj_inds.size());
			cam_pos.push_back(cam_from_world * obj->getPos());
		}
		
		std::sort(obj_inds.begin(), obj_inds.end(), [&](size_t l, size_t r)
		{
			return cam_pos[l][2] < cam_pos[r][2];
		});

		for (auto ind : obj_inds)
		{
			if (objects[ind]->isShow())
			{
				objects[ind]->render(this);
				objects_rendered++;
			}
		}
	}

	overlay_phase = true;

	glDisable(GL_DEPTH_TEST);

	for (auto& objects : overlay_object_layers)
	{
		removeDestroyedObjects(objects);

		for (const auto& obj : objects)
		{
			if (obj->isShow())
			{
				obj->render(this);
				objects_rendered++;
			}
		}
	}

	glEnable(GL_DEPTH_TEST);

	overlay_phase = false;
	
	// Swap buffers
	glfwSwapBuffers(window);

	while (true) {
		glfwPollEvents();

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			last_left_mouse_state = true;
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		{
			last_left_mouse_state = false;
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			last_right_mouse_state = true;
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
		{
			last_right_mouse_state = false;
		}

		if (fps > 0)
		{
			if (duration_cast<nanoseconds>(high_resolution_clock::now() - last_time).count() >= 1e9 / fps)
				break;
		}
		else
			break;
	}


	last_time = high_resolution_clock::now();
	
	bool close = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(window);
	
	if (close)
	{
		if (hide_on_close || ignore_close)
		{
			close = false;
			glfwSetWindowShouldClose(window, false);
			if (hide_on_close)
				hide();
		}
		else
		{
			shutdown();
		}
	}
	
	return !close;
}

void Window::shutdown()
{
	if (window == NULL)
		return;
	
	v4print "Window: Shutting down.";

	glfwMakeContextCurrent(window);
	
	for (auto& objects : object_layers)
	{
		for (const auto& obj : objects)
		{
			obj->setDestroy();
			obj->onRemoveFromWindow(this);
		}
	}

	for (auto& objects : overlay_object_layers)
	{
		for (const auto& obj : objects)
		{
			obj->setDestroy();
			obj->onRemoveFromWindow(this);
		}
	}
	
	window_top_right_node = NULL;
	window_bottom_right_node = NULL;
	window_top_left_node = NULL;
	window_bottom_left_node = NULL;
	object_layers.clear();
	overlay_object_layers.clear();
	non_render_objects.clear();
	
	if (first_window == window)
	{
		// Don't destroy this one, it owns the shared context
		glfwHideWindow(window);
	}
	else
		glfwDestroyWindow(window);
	window = NULL;
	
	v4print "Window: Shutdown complete.";
}

json Window::produceSceneJSON(vector<vector<char>>* bufs)
{
	json scene;
	
	scene["window"] = {
		{"width", window_size[0]},
		{"height", window_size[1]},
		{"title", title}
	};

	scene["objects"] = json();

	for (const auto& obj : non_render_objects)
	{
		json datum = obj->serialize(bufs);
		if (!datum.contains("id"))
			continue;
		datum["show"] = obj->isShow();
		if (obj->checkDestroy())
		{
			datum["destroy"] = true;
		}
		scene["objects"].push_back(datum);
	}

	for (int layer = 0; layer < object_layers.size(); layer++)
	{
		const auto& objects = object_layers[layer];
		for (const auto& obj : objects)
		{
			json datum = obj->serialize(bufs);
			if (!datum.contains("id"))
				continue;
			datum["show"] = obj->isShow();
			if (obj->checkDestroy())
			{
				datum["destroy"] = true;
			}
			datum["overlay"] = false;
			datum["layer"] = layer;
			scene["objects"].push_back(datum);
		}
	}

	for (int layer = 0; layer < overlay_object_layers.size(); layer++)
	{
		const auto& objects = overlay_object_layers[layer];
		for (const auto& obj : objects)
		{
			json datum = obj->serialize(bufs);
			if (!datum.contains("id"))
				continue;
			datum["show"] = obj->isShow();
			if (obj->checkDestroy())
			{
				datum["destroy"] = true;
			}
			datum["overlay"] = true;
			datum["layer"] = layer;
			scene["objects"].push_back(datum);
		}
	}

	return scene;
}

void Window::saveSceneArtifacts(const string& save_dir)
{
	int save_index = 0;

	for (int layer = 0; layer < object_layers.size(); layer++)
	{
		const auto& objects = object_layers[layer];
		for (const auto& obj : objects)
		{
			obj->saveArtifacts(save_dir+"/obj_"+to_string(save_index));
			save_index++;
		}
	}

	for (int layer = 0; layer < overlay_object_layers.size(); layer++)
	{
		const auto& objects = overlay_object_layers[layer];
		for (const auto& obj : objects)
		{
			obj->saveArtifacts(save_dir+"/obj_"+to_string(save_index));
			save_index++;
		}
	}
}

void setTextureSampling(bool nearest)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (nearest)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
}

shared_ptr<Texture> Window::loadTexture(const std::string& path, bool nearest){
	v4print "Loading texture:", path;
	
	if (!fs::exists(path))
		throw std::runtime_error("Texture does not exist at path: " + path);

	glfwMakeContextCurrent(window);
	
	int width, height, channels;
	const uint8_t* img = stbi_load(path.c_str(), &width, &height, &channels, 0);
	
	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	if (channels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	else if (channels == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	else
		throw std::runtime_error("Only supporting loading 3 and 4 channels from buffer");

	setTextureSampling(nearest);

	return make_shared<Texture>(textureID, Vec2i(width, height));
}

shared_ptr<Texture> Window::getTextureFromBuffer(const char* buf_data, int channels, const Vec2i& size, bool nearest)
{
	glfwMakeContextCurrent(window);

	int width = size[0];
	int height = size[1];
	
	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	if (channels == 1)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, buf_data);
	else if (channels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buf_data);
	else if (channels == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf_data);
	else
		throw std::runtime_error("Only supporting loading 1, 3, and 4 channels from buffer");

	setTextureSampling(nearest);

	return make_shared<Texture>(textureID, Vec2i(width, height));
}

shared_ptr<Texture> Window::getTextureFromImage(const Image<uint8_t>& img, bool nearest)
{
	const char* buf_data;
	int buf_size;
	img.getBuffer(buf_data, buf_size);

	return getTextureFromBuffer(buf_data, img.getChannels(), img.getSize(), nearest);
}

shared_ptr<Texture> Window::getTextureFromJSON(const json& data, int base_buf_index, const vector<vector<char>>& bufs, TexturePathResolveCallback path_callback, json* serial_header, vector<vector<char>>* serial_bufs)
{
	bool filter_nearest = false;
	if (data.contains("filter_nearest"))
		filter_nearest = data["filter_nearest"];

	if (data["type"] == "file")
	{
		string path = data["path"];
		if (path_callback)
			path = path_callback(path);
		return loadTexture(path, filter_nearest);
	}
	else if (data["type"] == "raw")
	{
		size_t buf_id = data["buf"];
		buf_id += base_buf_index;
		const auto& buf = bufs[buf_id];
		
		if (serial_header && serial_bufs)
		{
			(*serial_header)["buf"] = serial_bufs->size();
			serial_bufs->push_back(buf);
		}
		
		return getTextureFromBuffer(buf.data(), data["channels"], readVec2i(data["size"]), filter_nearest);
	}
	else if (data["type"] == "base64")
	{
		vector<uint8_t> bytes;
		macaron::Base64::Decode(data["data"], bytes);

		return getTextureFromBuffer(
			reinterpret_cast<const char*>(bytes.data()), 
			bytes.size(),
			readVec2i(data["size"]), 
			filter_nearest);
	}
	return NULL;
}

Image<uint8_t> Window::getScreenImage()
{
	glfwMakeContextCurrent(window);
	Image<uint8_t> image(window_size[0], window_size[1], 4);
	glReadPixels(0, 0, window_size[0], window_size[1], GL_RGBA, GL_UNSIGNED_BYTE, (uint8_t*)image.getData().data());

	image.flipYInPlace();

	return image;
}

Image<uint8_t> Window::getDepthImage()
{
	glfwMakeContextCurrent(window);
	Image<float> image(window_size[0], window_size[1], 1);
	glReadPixels(0, 0, window_size[0], window_size[1], GL_DEPTH_COMPONENT, GL_FLOAT, (float*)image.getData().data());
	
	//v4print "Depth range:", image.min(), image.max(); 

	float mult = 255.0 / (image.max() - image.min());

	//auto final_image = image.cast<uint8_t>(-255.0 / (image.max() - image.min()), 255.0);
	auto final_image = image.cast<uint8_t>(mult, -mult * image.min());
	
	//v4print "Depth range (uint8):", (int)final_image.min(), (int)final_image.max(); 

	final_image.flipYInPlace();
	
	return final_image;
}

}