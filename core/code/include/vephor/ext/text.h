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
 