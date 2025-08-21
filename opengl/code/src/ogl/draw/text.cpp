/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#include "vephor/ogl/draw/text.h"

namespace vephor
{
namespace ogl
{


string textVertexShader = R"(
    #version 430
    layout(location = 0) in vec3 pos_in_model;
	layout(location = 1) in vec2 in_uv;
    
    //uniform mat4 proj_from_model;

	uniform mat4 proj_from_camera;
	uniform mat4 cam_from_world;
    uniform mat4 world_from_model;
	uniform float scale;
	uniform float billboard;

	out vec2 uv;
    
    void main()
    {
		vec3 pos_in_camera = scale * pos_in_model * billboard + (cam_from_world * world_from_model * vec4(pos_in_model * (1-billboard),1)).xyz;
        gl_Position = proj_from_camera * vec4(pos_in_camera, 1);

		uv = in_uv;
    }
)";

string textFragmentShader = R"(
	#version 430

	in vec2 uv;

    out vec4 color;

	uniform vec3 mat_color;
	uniform sampler2D tex_sampler;

	void main()
	{
        color = texture( tex_sampler, uv );
		color *= vec4(mat_color, 1);
	}
)";

Text::Text(
    const string& p_text,
	const shared_ptr<Texture>& p_tex, 
    const Vec3& p_color,
	float p_x_border)
: color(p_color),
x_border(p_x_border),
tex(p_tex),
text(p_text)
{
}

Text::~Text()
{
}

void Text::onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
{
	program_id = buildProgram("text", textVertexShader, textFragmentShader);

    pos_attr_loc = glGetAttribLocation(program_id, "pos_in_model");
	uv_attr_loc = glGetAttribLocation(program_id, "in_uv");

    proj_matrix_id = glGetUniformLocation(program_id, "proj_from_camera");
	view_matrix_id = glGetUniformLocation(program_id, "cam_from_world");
	model_matrix_id = glGetUniformLocation(program_id, "world_from_model");
	scale_id = glGetUniformLocation(program_id, "scale");
	billboard_id = glGetUniformLocation(program_id, "billboard");
    color_id = glGetUniformLocation(program_id, "mat_color");
	tex_sampler_id  = glGetUniformLocation(program_id, "tex_sampler");
	
    glGenVertexArrays(1, &vao_id);

	setupText();
}

void Text::onRemoveFromWindow(Window*)
{
	glDeleteBuffers(1, &pos_buffer_id);
	glDeleteBuffers(1, &uv_buffer_id);
    glDeleteVertexArrays(1, &vao_id);
}

void Text::setupText()
{
	if (!tex)
	{
		throw std::runtime_error("Must set text texture before setting text.");
	}
	
	if (buffers_set)
	{
		glDeleteBuffers(1, &pos_buffer_id);
		glDeleteBuffers(1, &uv_buffer_id);
	}

	setAnchorOffset(anchor_perc);

	float size = 1.0f;

	verts.resize(3, text.size()*6);
	uvs.resize(2, text.size()*6);

	verts.setZero();

	float x = 0.0f;
	float y = 0.0f;

	float aspect = (1.0 - x_border);
	float cell_size_uv = 1.0f/16.0f;
	float border = cell_size_uv * (1.0f - aspect) / 2.0f;

	for (unsigned int i = 0; i < text.size(); i++)
	{	
		Vec2 vertex_up_left( x+i*size*aspect, y+size );
		Vec2 vertex_up_right( x+i*size*aspect+size*aspect, y+size );
		Vec2 vertex_down_right( x+i*size*aspect+size*aspect, y );
		Vec2 vertex_down_left( x+i*size*aspect, y );
		
		verts.col(i*6+0).head<2>() = vertex_up_left;
		verts.col(i*6+1).head<2>() = vertex_down_left;
		verts.col(i*6+2).head<2>() = vertex_up_right;

		verts.col(i*6+3).head<2>() = vertex_down_right;
		verts.col(i*6+4).head<2>() = vertex_up_right;
		verts.col(i*6+5).head<2>() = vertex_down_left;

		char character = text[i];
		float uv_x = (character%16)*cell_size_uv;
		float uv_y = (character/16)*cell_size_uv;

		Vec2 uv_up_left( uv_x+border, 1.0-uv_y );
		Vec2 uv_up_right( uv_x+cell_size_uv-border, 1.0-uv_y );
		Vec2 uv_down_right( uv_x+cell_size_uv-border, 1.0-(uv_y+cell_size_uv) );
		Vec2 uv_down_left( uv_x+border, 1.0-(uv_y+cell_size_uv) );

		if (y_flip)
		{
			std::swap(uv_up_left[1], uv_down_left[1]);
			std::swap(uv_up_right[1], uv_down_right[1]);
		}

		uvs.col(i*6+0) = uv_up_left;
		uvs.col(i*6+1) = uv_down_left;
		uvs.col(i*6+2) = uv_up_right;

		uvs.col(i*6+3) = uv_down_right;
		uvs.col(i*6+4) = uv_up_right;
		uvs.col(i*6+5) = uv_down_left;
	}

	for (int c = 0; c < verts.cols(); c++)
	{
		verts.col(c) += body_from_anchor.translation();
	}

	glBindVertexArray(vao_id);

	glGenBuffers(1, &pos_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, pos_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(
        pos_attr_loc,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
	glEnableVertexAttribArray(pos_attr_loc);

	glGenBuffers(1, &uv_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(GLfloat), uvs.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(
        uv_attr_loc,                                // attribute
        2,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
	glEnableVertexAttribArray(uv_attr_loc);

	glBindVertexArray(0);

	buffers_set = true;

	text_needs_setup = false;
}

void Text::renderOGL(Window* window, const TransformSim3& world_from_body)
{
	if (text_needs_setup)
		setupText();

	TransformSim3 world_from_anchor = world_from_body;// * body_from_anchor;
	
    glUseProgram(program_id);

	if (!tex)
    {
        tex = window->getDefaultTex();
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->getID());
    glUniform1i(tex_sampler_id, 0);

    Mat4 world_from_body_mat = world_from_anchor.matrix();
    Mat4 proj_from_camera = window->getProjectionMatrix();

    glUniformMatrix4fv(proj_matrix_id, 1, GL_FALSE, proj_from_camera.data());
	glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, world_from_body_mat.data());
    glUniformMatrix4fv(view_matrix_id, 1, GL_FALSE, window->getCamFromWorldMatrix().data());

    glUniform3fv(color_id, 1, color.data());
	glUniform1f(scale_id, world_from_anchor.scale);
	glUniform1f(billboard_id, billboard ? 1 : 0);
	
	glBindVertexArray(vao_id);

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArrays(GL_TRIANGLES, 0, verts.cols());

	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);

	glBindVertexArray(0);
}

} // namespace ogl
} // namespace vephor