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
#include "io.h"

namespace vephor
{

class Mesh
{
public:
    Mesh(
        const MeshData& data, 
        const Color& color = Color(1,1,1),
        float diffuse_strength = 1.0,
        float ambient_strength = 1.0,
		float emissive_strength = 0.0);
    ~Mesh();
	void setTexture(const shared_ptr<Texture>& p_tex){tex = p_tex;}
	void setNormalMap(const shared_ptr<Texture>& p_normal_map){normal_map = p_normal_map;}
	void setDiffuse(const Color& p_color){diffuse = p_color.getRGB();}
	Color getDiffuse() const {return diffuse;}
	void setAmbient(const Color& p_color){ambient = p_color.getRGB();}
	Color getAmbient() const {return ambient;}
	void setOpacity(const float& p_opacity){opacity = p_opacity;}
	void setCull(bool p_cull){cull = p_cull;}
    void setSpecular(bool p_specular){specular = p_specular;}
    void renderOGL(Window* window, const TransformSim3& world_from_body);
    void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node);
	void onRemoveFromWindow(Window* window);
private:
    MatX verts;
    MatX uvs;
    MatX norms;
	MatX tangents;
	MatX bitangents;
    Vec3 diffuse;
    Vec3 ambient;
	Vec3 emissive;
    float specular = 1.0f;
	float opacity = 1.0f;
    shared_ptr<Texture> tex;
	shared_ptr<Texture> normal_map;

    bool cull = true;
	
    GLuint vao_id;
    GLuint pos_buffer_id;
    GLuint uv_buffer_id;
    GLuint norm_buffer_id;
	GLuint tangent_buffer_id;
	GLuint bitangent_buffer_id;
    GLuint program_id;

    GLuint pos_attr_loc;
    GLuint uv_attr_loc;
    GLuint norm_attr_loc;
	GLuint tangent_attr_loc;
	GLuint bitangent_attr_loc;
    
    GLuint mvp_matrix_id;
	GLuint view_matrix_id;
	GLuint model_matrix_id;
	GLuint modelview_matrix_id;
    GLuint diffuse_id;
    GLuint ambient_id;
	GLuint emissive_id;
    GLuint specular_id;
	GLuint light_ambient_id;
    GLuint light_pos_id;
    GLuint light_dir_id;
    GLuint light_dir_strength_id;
    GLuint light_point_strength_id;
    GLuint opacity_id;
	GLuint tex_sampler_id;
	GLuint normal_sampler_id;
};

}