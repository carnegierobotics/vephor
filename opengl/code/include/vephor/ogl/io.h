/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
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
	void setRepeat()
	{
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
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

class CubeTexture
{
public:
    CubeTexture(GLuint p_id)
    : id(p_id)
    {
    }
    ~CubeTexture()
    {
        glDeleteTextures(1, &id);
    }
    GLuint getID() const {return id;}
private:
    GLuint id;
};

}