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

class Background
{
public:
    Background(
		const shared_ptr<Texture>& tex
    );
    ~Background();
	void setScale(float p_scale){scale = p_scale;}
	float getScale() const {return scale;}
    void renderOGL(Window* window, const TransformSim3& world_from_body);
    void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node);
	void onRemoveFromWindow(Window* window);
private:
    MatX verts;
	MatX uvs;
    float scale = 1.0f;
    shared_ptr<Texture> tex;

    GLuint vao_id;
    GLuint pos_buffer_id;
	GLuint uv_buffer_id;
    GLuint program_id;

    GLuint pos_attr_loc;
	GLuint uv_attr_loc;

	GLuint tex_sampler_id;
};

} // namespace ogl
} // namespace vephor