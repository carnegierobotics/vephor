#include "vephor/ogl/instanced_points.h"

namespace vephor
{

string instancedPointsVertexShader = R"(
    #version 430
    layout(location = 0) in vec3 pos_in_model;
	layout(location = 1) in vec2 in_uv;
	layout(location = 2) in vec3 offset;
	layout(location = 3) in vec4 color;
    
    uniform mat4 proj_from_camera;
	uniform mat4 cam_from_model;
	uniform float size;
	out vec2 uv;
	out vec4 col;
    
    void main()
    {
		vec3 pos_in_camera = size * pos_in_model + (cam_from_model * vec4(offset,1)).xyz;
		
        gl_Position = proj_from_camera * vec4(pos_in_camera, 1);
        
		uv = in_uv;
		col = color;
    }
)";

string instancedPointsFragmentShader = R"(
	#version 430

    in vec2 uv;
	in vec4 col;

    out vec4 color;

	uniform sampler2D tex_sampler;

	void main()
	{
        vec4 tex_rgba = texture( tex_sampler, uv );
        color = tex_rgba * col;
		
		if (color.a < 0.01f)
			discard;
	}
)";

string instancedPointsScreenSpaceScaleVertexShader = R"(
    #version 430
    layout(location = 0) in vec3 pos_in_model;
	layout(location = 1) in vec2 in_uv;
	layout(location = 2) in vec3 offset;
	layout(location = 3) in vec4 color;
    
    uniform mat4 proj_from_camera;
	uniform mat4 cam_from_model;
	uniform float size;
	uniform float aspect;
	out vec2 uv;
	out vec4 col;
    
    void main()
    {
		vec3 center_in_camera = (cam_from_model * vec4(offset,1)).xyz;
		
		vec4 center_in_proj = proj_from_camera * vec4(center_in_camera, 1);
		vec4 ss_offset = vec4(size * 2 * pos_in_model.x / aspect, size * 2 * pos_in_model.y, 0.0, 0.0) * center_in_proj.w; 
        gl_Position = center_in_proj + ss_offset;
        
		uv = in_uv;
		col = color;
    }
)";

InstancedPoints::InstancedPoints(
	const MatXRef& p_pts,
    const MatXRef& p_colors,
	const Vec4& p_default_color)
: offsets(p_pts.transpose())
{
    colors.resize(4,p_pts.rows());
	
	if (p_colors.cols() == 3 && p_colors.rows() > 0)
	{
        colors.block(0,0,3,p_colors.rows()) = p_colors.transpose();
		colors.block(3,0,1,p_colors.rows()).fill(1);
	}
    else if (p_colors.cols() == 4 && p_colors.rows() > 0)
        colors.block(0,0,4,p_colors.rows()) = p_colors.transpose();
    colors.block(0,p_colors.rows(),4,p_pts.rows()-p_colors.rows()).colwise() = p_default_color;
	
	verts.resize(6,3);
	uvs.resize(6,2);
	
	verts.row(0) = Vec3(0,0,0);
	verts.row(1) = Vec3(0,1,0);
	verts.row(2) = Vec3(1,1,0);
	verts.row(3) = Vec3(0,0,0);
	verts.row(4) = Vec3(1,1,0);
	verts.row(5) = Vec3(1,0,0);
	
	Vec3 center(0.5f,0.5f,0);
	
	for (int i = 0; i < 6; i++)
		verts.row(i) -= center;
	
	uvs.row(0) = Vec2(0,1);
	uvs.row(1) = Vec2(0,0);
	uvs.row(2) = Vec2(1,0);
	uvs.row(3) = Vec2(0,1);
	uvs.row(4) = Vec2(1,0);
	uvs.row(5) = Vec2(1,1);
	
	verts = verts.transpose().eval();
	uvs = uvs.transpose().eval();

    program_id = buildProgram("InstancedPoints", instancedPointsVertexShader, instancedPointsFragmentShader);
	ss_program_id = buildProgram("InstancedPointsScreenSpace", instancedPointsScreenSpaceScaleVertexShader, instancedPointsFragmentShader);

    
	pos_attr_loc = glGetAttribLocation(program_id, "pos_in_model");
	uv_attr_loc = glGetAttribLocation(program_id, "in_uv");
	offset_attr_loc = glGetAttribLocation(program_id, "offset");
	color_attr_loc = glGetAttribLocation(program_id, "color");

    mvp_matrix_id = glGetUniformLocation(program_id, "proj_from_camera");
	modelview_matrix_id = glGetUniformLocation(program_id, "cam_from_model");
	size_id = glGetUniformLocation(program_id, "size");
	tex_sampler_id  = glGetUniformLocation(program_id, "tex_sampler");
	

	ss_pos_attr_loc = glGetAttribLocation(ss_program_id, "pos_in_model");
	ss_uv_attr_loc = glGetAttribLocation(ss_program_id, "in_uv");
	ss_offset_attr_loc = glGetAttribLocation(ss_program_id, "offset");
	ss_color_attr_loc = glGetAttribLocation(ss_program_id, "color");

    ss_mvp_matrix_id = glGetUniformLocation(ss_program_id, "proj_from_camera");
	ss_modelview_matrix_id = glGetUniformLocation(ss_program_id, "cam_from_model");
	ss_size_id = glGetUniformLocation(ss_program_id, "size");
	ss_aspect_id = glGetUniformLocation(ss_program_id, "aspect");
	ss_tex_sampler_id  = glGetUniformLocation(ss_program_id, "tex_sampler");
	

	
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

	glGenBuffers(1, &pos_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, pos_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &uv_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(GLfloat), uvs.data(), GL_STATIC_DRAW);
	
	glGenBuffers(1, &offset_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, offset_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, offsets.size() * sizeof(GLfloat), offsets.data(), GL_STATIC_DRAW);
	
	glGenBuffers(1, &color_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), colors.data(), GL_STATIC_DRAW);
}

InstancedPoints::~InstancedPoints()
{
	glBindVertexArray(vao_id);
    glDeleteBuffers(1, &pos_buffer_id);
    glDeleteBuffers(1, &uv_buffer_id);
	glDeleteBuffers(1, &offset_buffer_id);
	glDeleteBuffers(1, &color_buffer_id);
    glDeleteVertexArrays(1, &vao_id);
}

void InstancedPoints::renderOGL(Window* window, const TransformSim3& world_from_body)
{
	glBindVertexArray(vao_id);
	
	if (ss_mode)
		glUseProgram(ss_program_id);
	else
		glUseProgram(program_id);

	if (!tex)
    {
        tex = window->getDefaultTex();
    }

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->getID());

	Mat4 world_from_body_mat = world_from_body.matrix();
	
	Mat4 cam_from_world_mat = window->getCamFromWorldMatrix();
	
    Mat4 MVP = window->getProjectionMatrix();// * cam_from_world_mat * world_from_body_mat;
	
	Mat4 cam_from_body_mat = cam_from_world_mat * world_from_body_mat;
	
	GLuint curr_pos_attr_loc;
	GLuint curr_uv_attr_loc;
	GLuint curr_offset_attr_loc;
	GLuint curr_color_attr_loc;
	
	if (ss_mode)
	{
		glUniform1i(ss_tex_sampler_id, 0);
		glUniformMatrix4fv(ss_mvp_matrix_id, 1, GL_FALSE, MVP.data());
		glUniformMatrix4fv(ss_modelview_matrix_id, 1, GL_FALSE, cam_from_body_mat.data());
		glUniform1f(ss_size_id, size * world_from_body.scale);
		glUniform1f(ss_aspect_id, (float)window->getSize()[0] / (float)window->getSize()[1]);
		
		curr_pos_attr_loc = ss_pos_attr_loc;
		curr_uv_attr_loc = ss_uv_attr_loc;
		curr_offset_attr_loc = ss_offset_attr_loc;
		curr_color_attr_loc = ss_color_attr_loc;
	}
	else
	{
		glUniform1i(tex_sampler_id, 0);
		glUniformMatrix4fv(mvp_matrix_id, 1, GL_FALSE, MVP.data());
		glUniformMatrix4fv(modelview_matrix_id, 1, GL_FALSE, cam_from_body_mat.data());
		glUniform1f(size_id, size * world_from_body.scale);
		
		curr_pos_attr_loc = pos_attr_loc;
		curr_uv_attr_loc = uv_attr_loc;
		curr_offset_attr_loc = offset_attr_loc;
		curr_color_attr_loc = color_attr_loc;
	}

	glBindVertexArray(vao_id);
	
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, pos_buffer_id);
    glVertexAttribPointer(
        curr_pos_attr_loc,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_id);
    glVertexAttribPointer(
        curr_uv_attr_loc,                                // attribute
        2,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
	
	glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, offset_buffer_id);
	glVertexAttribPointer(
        curr_offset_attr_loc,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
	glVertexAttribDivisor(2, 1);
	
	glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id);
	glVertexAttribPointer(
        curr_color_attr_loc,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        4,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
	glVertexAttribDivisor(3, 1);


	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glDrawArraysInstanced(GL_TRIANGLES, 0, verts.cols(), offsets.cols());
	
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}

}