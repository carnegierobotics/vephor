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

#include "vephor/ogl/points.h"

namespace vephor
{

string pointsVertexShader = R"(
    #version 430
    layout(location = 0) in vec3 pos_in_model;
	
    uniform mat4 proj_from_camera;
	uniform mat4 cam_from_model;
    
    void main()
    {
		gl_Position = (proj_from_camera * cam_from_model * vec4(pos_in_model,1));
    }
)";

string pointsFragmentShader = R"(
	#version 430

    out vec3 color;

	uniform vec3 mat_color;

	void main()
	{
        color = mat_color;
	}
)";

Points::Points(
	const vector<Vec3>& p_pts)
{
	verts.resize(p_pts.size(),3);
	
	for (size_t i = 0; i < p_pts.size(); i++)
		verts.row(i) = p_pts[i];
	
	verts = verts.transpose().eval();
}

Points::~Points()
{
}

void Points::onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
{
    program_id = buildProgram("points", pointsVertexShader, pointsFragmentShader);

    // Get the 'pos' variable location inside this program
    pos_attr_loc = glGetAttribLocation(program_id, "pos_in_model");

    mvp_matrix_id = glGetUniformLocation(program_id, "proj_from_camera");
	modelview_matrix_id = glGetUniformLocation(program_id, "cam_from_model");
    color_id = glGetUniformLocation(program_id, "mat_color");
     
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

	glGenBuffers(1, &pos_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, pos_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);
}

void Points::onRemoveFromWindow(Window*)
{
    glDeleteBuffers(1, &pos_buffer_id);
    glDeleteVertexArrays(1, &vao_id);
}

void Points::renderOGL(Window* window, const TransformSim3& world_from_body)
{
	glBindVertexArray(vao_id);
	
    glUseProgram(program_id);

	Mat4 world_from_body_mat = world_from_body.matrix();
	
	Mat4 cam_from_world_mat = window->getCamFromWorldMatrix();
	
    Mat4 MVP = window->getProjectionMatrix();// * cam_from_world_mat * world_from_body_mat;

    glUniformMatrix4fv(mvp_matrix_id, 1, GL_FALSE, MVP.data());
	
	Mat4 cam_from_body_mat = cam_from_world_mat * world_from_body_mat;
    glUniformMatrix4fv(modelview_matrix_id, 1, GL_FALSE, cam_from_body_mat.data());

    glUniform3fv(color_id, 1, color.data());
    
	glBindVertexArray(vao_id);
	
    glEnableVertexAttribArray(pos_attr_loc);
    glBindBuffer(GL_ARRAY_BUFFER, pos_buffer_id);
    glVertexAttribPointer(
        pos_attr_loc,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

	glDisable(GL_CULL_FACE);

	glDrawArrays(GL_POINTS, 0, verts.cols());
	
	glEnable(GL_CULL_FACE);

    glDisableVertexAttribArray(pos_attr_loc);
}

}