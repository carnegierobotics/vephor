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

class Text
{
public:
    Text(const string& p_text)
	: text(p_text)
	{
	}
	void setColor(const Color& p_color){color = p_color.getRGB();}
	json serialize(vector<vector<char>>*)
	{
		return {
            {"type", "text"},
			{"text", text},
			{"anchor", toJson(anchor)},
			{"color_rgb", toJson(color)}
        };
	}
	
	void setAnchorBottomLeft()
	{
		anchor = Vec2(0,0);
	}
	void setAnchorLeft()
	{
		anchor = Vec2(0,0.5);
	}
	void setAnchorTopLeft()
	{
		anchor = Vec2(0,1);
	}
	void setAnchorBottom()
	{
		anchor = Vec2(0.5,0);
	}
	void setAnchorCentered()
	{
		anchor = Vec2(0.5,0.5);
	}
	void setAnchorTop()
	{
		anchor = Vec2(0.5,1);
	}
	void setAnchorBottomRight()
	{
		anchor = Vec2(1,0);
	}
	void setAnchorRight()
	{
		anchor = Vec2(1,0.5);
	}
	void setAnchorTopRight()
	{
		anchor = Vec2(1,1);
	}
private:
	string text;
	Vec2 anchor = Vec2::Zero();
	Vec3 color = Vec3(1,1,1);
};

}
 