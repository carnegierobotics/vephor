#include "vephor/ogl/io.h"

namespace vephor
{

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

shared_ptr<Texture> loadTexture(const std::string& path, bool nearest){
	v4print "Loading texture:", path;
	
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

shared_ptr<Texture> getTextureFromBuffer(const char* buf_data, int channels, const Vec2i& size, bool nearest)
{
	int width = size[0];
	int height = size[1];
	
	v4print "Image from buffer:", width, height, channels;
	
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

shared_ptr<Texture> getTextureFromImage(const Image<uint8_t>& img, bool nearest)
{
	const char* buf_data;
	int buf_size;
	img.getBuffer(buf_data, buf_size);

	return getTextureFromBuffer(buf_data, img.getChannels(), img.getSize(), nearest);
}

shared_ptr<Texture> getTextureFromJSON(const json& data, int base_buf_index, const vector<vector<char>>& bufs, TexturePathResolveCallback path_callback, json* serial_header, vector<vector<char>>* serial_bufs)
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

}