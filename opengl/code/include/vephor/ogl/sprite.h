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
    Sprite(
		const shared_ptr<Texture>& sprite_sheet,
        Vec2i px_per_cell = Vec2i::Zero(),
		bool x_flip = false,
		bool y_flip = false
    );
    ~Sprite();
	void setCell(int cell){curr_cell = cell;}
	void setSpriteSheet(const shared_ptr<Texture>& p_sprite_sheet){sprite_sheet = p_sprite_sheet;}
    void setNormalSpriteSheet(const shared_ptr<Texture>& p_normal_sprite_sheet){normal_sprite_sheet = p_normal_sprite_sheet;}
	void setDiffuse(const Vec3& p_color){diffuse = p_color;}
	void setAmbient(const Vec3& p_color){ambient = p_color;}
	void setEmissive(const Vec3& p_color){emissive = p_color;}
	void setOpacity(const float& p_opacity){opacity = p_opacity;}
    void renderOGL(Window* window, const TransformSim3& world_from_body);
	void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node);
	void onRemoveFromWindow(Window* window);
private:
    MatX verts;
	MatX uvs;
	Vec3 diffuse;
    Vec3 ambient;
	Vec3 emissive = Vec3::Zero();
	float opacity = 1.0f;
    shared_ptr<Texture> sprite_sheet;
    shared_ptr<Texture> normal_sprite_sheet;
	Vec2 uv_scale;
	vector<Vec2> uvs_per_cell;

	size_t curr_cell = 0;

    GLuint vao_id;
    GLuint pos_buffer_id;
	GLuint uv_buffer_id;
    GLuint program_id;

    GLuint pos_attr_loc;
	GLuint uv_attr_loc;

    GLuint proj_matrix_id;
    GLuint view_matrix_id;
	GLuint inv_view_matrix_id;
	GLuint model_matrix_id;
    GLuint scale_id;
    GLuint diffuse_id;
    GLuint ambient_id;
	GLuint emissive_id;
    GLuint light_pos_id;
    GLuint light_dir_id;
    GLuint light_dir_strength_id;
    GLuint light_point_strength_id;
	GLuint opacity_id;
	GLuint uv_scale_id;
	GLuint uv_offset_id;
	GLuint tex_sampler_id;
    GLuint normal_sampler_id;
};

}