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

class Sprite
{
public:
    Sprite(
		const shared_ptr<Texture>& sprite_sheet,
        Vec2i px_per_cell = Vec2i::Zero(),
		bool x_flip = false,
		bool y_flip = false,
		bool xy_swap = false
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

} // namespace ogl
} // namespace vephor