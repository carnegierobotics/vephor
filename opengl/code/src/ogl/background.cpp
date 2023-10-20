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

#include "vephor/ogl/background.h"

namespace vephor
{

string backgroundVertexShader = R"(
    #version 430
    layout(location = 0) in vec3 pos_in_model;
	layout(location = 1) in vec2 in_uv;
    
	out vec2 uv;
    
    void main()
    {
        gl_Position = vec4(pos_in_model, 1);
		uv = in_uv;
    }
)";

string backgroundFragmentShader = R"(
	#version 430

    in vec2 uv;

    out vec4 color;

	uniform sampler2D tex_sampler;

	void main()
	{
        color = texture( tex_sampler, uv );
	}
)";

Background::Background(
    const shared_ptr<Texture>& p_tex)
: tex(p_tex)
{
    verts.resize(6,3);
    uvs.resize(6,2);

    verts.row(0) = Vec3(-1,-1,0);
    verts.row(1) = Vec3(-1,1,0);
    verts.row(2) = Vec3(1,1,0);
    verts.row(3) = Vec3(-1,-1,0);
    verts.row(4) = Vec3(1,1,0);
    verts.row(5) = Vec3(1,-1,0);

    uvs.row(0) = Vec2(0,1);
	uvs.row(1) = Vec2(0,0);
	uvs.row(2) = Vec2(1,0);
	uvs.row(3) = Vec2(0,1);
	uvs.row(4) = Vec2(1,0);
	uvs.row(5) = Vec2(1,1);

    verts = verts.transpose().eval();
	uvs = uvs.transpose().eval();
}

Background::~Background()
{ 
}

void Background::onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
{
    program_id = buildProgram("background", backgroundVertexShader, backgroundFragmentShader);
    
    pos_attr_loc = glGetAttribLocation(program_id, "pos_in_model");
	uv_attr_loc = glGetAttribLocation(program_id, "in_uv");

	tex_sampler_id  = glGetUniformLocation(program_id, "tex_sampler");

    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

	glGenBuffers(1, &pos_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, pos_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(
        pos_attr_loc,   
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
}

void Background::onRemoveFromWindow(Window*)
{
    glDeleteBuffers(1, &pos_buffer_id);
    glDeleteBuffers(1, &uv_buffer_id);
    glDeleteVertexArrays(1, &vao_id);
}

void Background::renderOGL(Window* window, const TransformSim3& world_from_body)
{
    glUseProgram(program_id);

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->getID());
    glUniform1i(tex_sampler_id, 0);

	glBindVertexArray(vao_id);
	
    /*glEnableVertexAttribArray(pos_attr_loc);
    glBindBuffer(GL_ARRAY_BUFFER, pos_buffer_id);
    glVertexAttribPointer(
        pos_attr_loc,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    glEnableVertexAttribArray(uv_attr_loc);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_id);
    glVertexAttribPointer(
        uv_attr_loc,                                // attribute
        2,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );*/

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
    //glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    //glDepthFunc(GL_ALWAYS);

    glDrawArrays(GL_TRIANGLES, 0, verts.cols());
	
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
    //glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    //glDepthFunc(GL_LESS);

    //glDisableVertexAttribArray(pos_attr_loc);
    //glDisableVertexAttribArray(uv_attr_loc);

    glBindVertexArray(0);
}

}