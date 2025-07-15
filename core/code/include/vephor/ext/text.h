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
		json ser = {
            {"type", "text"},
			{"text", text}
        };

		if (anchor != Vec2::Zero())
			ser["anchor"] = toJson(anchor);

		if (color != Vec3(1,1,1))
			ser["color_rgb"] = toJson(color);

		if (billboard != true)
			ser["billboard"] = billboard;

		if (y_flip != false)
			ser["y_flip"] = y_flip;

		return ser;
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
	void setBillboard(bool p_billboard)
	{
		billboard = p_billboard;
	}
	void setYFlip(bool p_y_flip)
	{
		y_flip = p_y_flip;
	}
private:
	string text;
	
	Vec2 anchor = Vec2::Zero();
	Vec3 color = Vec3(1,1,1);
	bool billboard = true;
	bool y_flip = false;
};

}
 