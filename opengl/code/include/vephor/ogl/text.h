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
		
		body_from_anchor.translation(Vec3(anchor_offset[0], anchor_offset[1], 0));
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

    bool buffers_set = false;
    GLuint vao_id;
    GLuint pos_buffer_id;
    GLuint uv_buffer_id;
    GLuint program_id;

    GLuint pos_attr_loc;
    GLuint uv_attr_loc;

    GLuint mvp_matrix_id;
    GLuint view_matrix_id;
    GLuint model_matrix_id;
    GLuint scale_id;
	GLuint billboard_id;
    GLuint color_id;
    GLuint tex_sampler_id;
};

}