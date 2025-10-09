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

#include "../window.h"

namespace vephor
{
namespace ogl
{

class Text
{
public:
    Text(
        const string& text,
		const shared_ptr<Texture>& tex,
        const Vec3& color = Vec3(1,1,1),
		float x_border = 0.5f
    );
    ~Text();
    void setText(const string& p_text)
	{
		text_needs_setup = true;
		text = p_text;
	}
	void setColor(const Vec3& p_color){color = p_color;}
    void renderOGL(Window* window, const TransformSim3& world_from_body);
	
	void setAnchorOffset(const Vec2& p_anchor_perc)
	{
		anchor_perc = p_anchor_perc;
		
		float letter_width = (1.0 - x_border);
		Vec2 size(letter_width * text.size(), 1.0);
		
		Vec2 anchor_offset(
			-size[0] * p_anchor_perc[0],
			-size[1] * p_anchor_perc[1]
		);
		
		body_from_anchor.setTranslation(Vec3(anchor_offset[0], anchor_offset[1], 0));
	}
	
	void setAnchorBottomLeft()
	{
		setAnchorOffset(Vec2(0,0));
	}
	void setAnchorLeft()
	{
		setAnchorOffset(Vec2(0,0.5));
	}
	void setAnchorTopLeft()
	{
		setAnchorOffset(Vec2(0,1));
	}
	void setAnchorBottom()
	{
		setAnchorOffset(Vec2(0.5,0));
	}
	void setAnchorCentered()
	{
		setAnchorOffset(Vec2(0.5,0.5));
	}
	void setAnchorTop()
	{
		setAnchorOffset(Vec2(0.5,1));
	}
	void setAnchorBottomRight()
	{
		setAnchorOffset(Vec2(1,0));
	}
	void setAnchorRight()
	{
		setAnchorOffset(Vec2(1,0.5));
	}
	void setAnchorTopRight()
	{
		setAnchorOffset(Vec2(1,1));
	}
	
	Vec2 getSize() const
	{
		float letter_width = (1.0 - x_border);
		Vec2 size(letter_width * text.size(), 1.0);
		return size;
	}
    void setTexture(const shared_ptr<Texture>& p_tex)
    {
        tex = p_tex;
    }
	void setBillboard(bool p_billboard)
	{
		billboard = p_billboard;
	}
	void setYFlip(bool p_y_flip)
	{
		y_flip = p_y_flip;
	}
	void setXYSwap(bool p_xy_swap)
	{
		xy_swap = p_xy_swap;
	}
	void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node);
	void onRemoveFromWindow(Window* window);
private:
	bool text_needs_setup = true;
	void setupText();

    MatX verts;
    MatX uvs;
	Vec3 anchor_offset = Vec3::Zero();
    Vec3 color;
	float x_border;
    shared_ptr<Texture> tex;
	string text;
	Vec2 anchor_perc = Vec2::Zero();
	TransformSim3 body_from_anchor;
	bool billboard = true;
	bool y_flip = false;
	bool xy_swap = false;

    bool buffers_set = false;
    GLuint vao_id = std::numeric_limits<GLuint>::max();
    GLuint pos_buffer_id = std::numeric_limits<GLuint>::max();
    GLuint uv_buffer_id = std::numeric_limits<GLuint>::max();
    GLuint program_id = std::numeric_limits<GLuint>::max();

    GLuint pos_attr_loc = std::numeric_limits<GLuint>::max();
    GLuint uv_attr_loc = std::numeric_limits<GLuint>::max();

    GLuint proj_matrix_id = std::numeric_limits<GLuint>::max();
    GLuint view_matrix_id = std::numeric_limits<GLuint>::max();
    GLuint model_matrix_id = std::numeric_limits<GLuint>::max();
    GLuint scale_id = std::numeric_limits<GLuint>::max();
	GLuint billboard_id = std::numeric_limits<GLuint>::max();
    GLuint color_id = std::numeric_limits<GLuint>::max();
    GLuint tex_sampler_id = std::numeric_limits<GLuint>::max();
};

} // namespace ogl
} // namespace vephor