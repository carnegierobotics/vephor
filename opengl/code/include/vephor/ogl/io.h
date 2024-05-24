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
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace vephor
{
class Texture
{
public:
    Texture(GLuint p_id, const Vec2i& p_size_wh)
    : id(p_id), size_wh(p_size_wh)
    {
    }
    ~Texture()
    {
        glDeleteTextures(1, &id);
    }
	void setRepeatX()
	{
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	}
	void setRepeatY()
	{
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
    GLuint getID() const {return id;}
	int rows() const {return size_wh[1];}
	int cols() const {return size_wh[0];}
	int width() const {return size_wh[0];}
	int height() const {return size_wh[1];}
	Vec2i size() const {return size_wh;}
private:
    GLuint id;
	Vec2i size_wh;
};

}