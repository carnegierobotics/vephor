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

class Particle
{
public:
    Particle(
        const vector<Vec3>& pts
    );
    ~Particle();
	void setTexture(const shared_ptr<Texture>& p_tex){tex = p_tex;}
	void setColor(const Vec3& p_color){color = p_color;}
	void setSize(float p_size){size = p_size;}
	void setOpacity(const float& p_opacity){opacity = p_opacity;}
    void renderOGL(Window* window, const TransformSim3& world_from_body);
	void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node);
	void onRemoveFromWindow(Window* window);
private:
    MatX verts;
	MatX uvs;
	Vec3 color = Vec3(1,1,1);
	float size = 0.03f;
	float opacity = 1.0f;
    shared_ptr<Texture> tex;
    vector<Vec3> pts;

    GLuint vao_id;
    GLuint pos_buffer_id;
	GLuint uv_buffer_id;
    GLuint program_id;

    GLuint pos_attr_loc;
	GLuint uv_attr_loc;
	
	GLuint mvp_matrix_id;
    GLuint modelview_matrix_id;
	GLuint size_id;
    GLuint color_id;
	GLuint opacity_id;
	GLuint offset_id;
	GLuint tex_sampler_id;
};

}