/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

#include "vephor/ogl/lines.h"

namespace vephor
{


string linesVertexShader = R"(
    #version 430
    layout(location = 0) in vec3 pos_in_model;
    layout(location = 1) in vec4 color;
    
    uniform mat4 proj_from_model;
    out vec4 col;
    
    void main()
    {
        gl_Position = proj_from_model * vec4(pos_in_model, 1);
        col = color;
    }
)";

string linesFragmentShader = R"(
	#version 430

    in vec4 col;

    out vec4 color;

	void main()
	{
        color = col;

        if (color.a < 0.01f)
			discard;
	}
)";

Lines::Lines(
    const MatXRef& p_verts, 
    const MatXRef& p_colors,
	const Color& p_default_color)
: verts(p_verts.transpose())
{
    colors.resize(4,p_verts.rows());
	
    if (p_colors.rows() > 0)
    {
        if (p_colors.cols() == 4)
            colors.block(0,0,4,p_colors.rows()) = p_colors.transpose();
        else if (p_colors.cols() == 3)
        {
            colors.block(0,0,3,p_colors.rows()) = p_colors.transpose();
            colors.block(3,0,1,p_colors.rows()).fill(1);
        }
    }

    colors.block(0,p_colors.rows(),4,p_verts.rows()-p_colors.rows()).colwise() = p_default_color.getRGBA();
}

Lines::~Lines()
{
}

void Lines::onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
{
    program_id = buildProgram("lines", linesVertexShader, linesFragmentShader);

    // Get the 'pos' variable location inside this program
    pos_attr_loc = glGetAttribLocation(program_id, "pos_in_model");
    color_attr_loc = glGetAttribLocation(program_id, "color");

    mvp_matrix_id = glGetUniformLocation(program_id, "proj_from_model");

    glGenVertexArrays(1, &vao_id);
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

    glGenBuffers(1, &color_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), colors.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(
        color_attr_loc,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        4,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    glEnableVertexAttribArray(color_attr_loc);

    glBindVertexArray(0);
}

void Lines::onRemoveFromWindow(Window*)
{
    glDeleteBuffers(1, &pos_buffer_id);
    glDeleteBuffers(1, &color_buffer_id);
    glDeleteVertexArrays(1, &vao_id);
}

void Lines::renderOGL(Window* window, const TransformSim3& world_from_body)
{
    glUseProgram(program_id);

    Mat4 world_from_body_mat = world_from_body.matrix();

    Mat4 MVP = window->getProjectionMatrix() * window->getCamFromWorldMatrix() * world_from_body_mat;

    glUniformMatrix4fv(mvp_matrix_id, 1, GL_FALSE, MVP.data());

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

    glEnableVertexAttribArray(color_attr_loc);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id);
	glVertexAttribPointer(
        color_attr_loc,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        4,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );*/

    if (is_strip)
        glDrawArrays(GL_LINE_STRIP, 0, verts.cols());
    else
        glDrawArrays(GL_LINES, 0, verts.cols());

    //glDisableVertexAttribArray(pos_attr_loc);
    //glDisableVertexAttribArray(color_attr_loc);

    glBindVertexArray(0);
}

}