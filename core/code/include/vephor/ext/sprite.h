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

#include "window.h"

namespace vephor
{

class Sprite
{
public:
    Sprite(const string& p_tex, bool p_filter_nearest = false)
	{
		tex_data.path = p_tex;
		tex_data.filter_nearest = p_filter_nearest;
	}
	Sprite(const Image<uint8_t>& p_tex, bool p_filter_nearest = false)
	{
		if (p_tex.getChannels() == 1)
		{
			tex_data.tex = p_tex.changeChannels(3);
		}
		else
		{
			tex_data.tex = p_tex;
		}

		tex_data.filter_nearest = p_filter_nearest;
	}
	void setColor(const Color& p_color){color = p_color.getRGB();}
	void setFlip(bool p_x_flip, bool p_y_flip)
	{
		x_flip = p_x_flip;
		y_flip = p_y_flip;
	}
	void setNormalSpriteSheet(const string& p_tex, bool p_filter_nearest = false)
	{
		normal_tex_data.path = p_tex;
		normal_tex_data.filter_nearest = p_filter_nearest;
	}
	void setNormalSpriteSheet(const Image<uint8_t>& p_tex, bool p_filter_nearest = false)
	{
		normal_tex_data.tex = p_tex;
		normal_tex_data.filter_nearest = p_filter_nearest;
	}
	json serialize(vector<vector<char>>* bufs)
	{
		json data = {
            {"type", "sprite"},
			{"tex", produceTextureData(tex_data, bufs)},
			{"normal_tex", produceTextureData(normal_tex_data, bufs)},
			{"x_flip", x_flip},
			{"y_flip", y_flip},
			{"color_rgb", toJson(color)},
			{"diffuse", diffuse_strength},
			{"ambient", ambient_strength},
        };
		
		return data;
	}
private:
	TextureDataRecord tex_data;
	TextureDataRecord normal_tex_data;
	bool x_flip = false;
	bool y_flip = false;
	Vec3 color = Vec3(1,1,1);
	float diffuse_strength = 1.0f;
	float ambient_strength = 1.0f;
};

}