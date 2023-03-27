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