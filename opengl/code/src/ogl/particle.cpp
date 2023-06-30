#include "vephor/ogl/particle.h"

namespace vephor
{

string particleVertexShader = R"(
    #version 430
    layout(location = 0) in vec3 pos_in_model;
	layout(location = 1) in vec2 in_uv;
    
    uniform mat4 proj_from_camera;
	uniform mat4 cam_from_model;
	uniform float size;
	uniform vec3 offset;
	out vec2 uv;
    
    void main()
    {
		vec3 pos_in_camera = size * pos_in_model + (cam_from_model * vec4(offset,1)).xyz;
		
        gl_Position = proj_from_camera * vec4(pos_in_camera, 1);
        
		uv = in_uv;
    }
)";

string particleFragmentShader = R"(
	#version 430

    in vec2 uv;

    out vec4 color;

	uniform vec3 mat_color;
	uniform float opacity = 1.0f;
	uniform sampler2D tex_sampler;

	void main()
	{
        vec4 tex_rgba = texture( tex_sampler, uv );
        color.rgb = tex_rgba.rgb * mat_color;
		color.a = tex_rgba.a * opacity;
		
		//color.rgb = mat_color;
		//color.a = opacity;
		
		if (color.a < 0.01f)
			discard;
	}
)";

Particle::Particle(
	const vector<Vec3>& p_pts)
: pts(p_pts)
{
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
}

Particle::~Particle()
{
}

void Particle::onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
{
	program_id = buildProgram("particle", particleVertexShader, particleFragmentShader);

    // Get the 'pos' variable location inside this program
    pos_attr_loc = glGetAttribLocation(program_id, "pos_in_model");
	uv_attr_loc = glGetAttribLocation(program_id, "in_uv");

    mvp_matrix_id = glGetUniformLocation(program_id, "proj_from_camera");
	modelview_matrix_id = glGetUniformLocation(program_id, "cam_from_model");
	size_id = glGetUniformLocation(program_id, "size");
    color_id = glGetUniformLocation(program_id, "mat_color");
    opacity_id = glGetUniformLocation(program_id, "opacity");
	offset_id = glGetUniformLocation(program_id, "offset");
	tex_sampler_id  = glGetUniformLocation(program_id, "tex_sampler");
	 
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

	glGenBuffers(1, &pos_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, pos_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &uv_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(GLfloat), uvs.data(), GL_STATIC_DRAW);
}

void Particle::onRemoveFromWindow(Window*)
{
	glDeleteBuffers(1, &pos_buffer_id);
    glDeleteBuffers(1, &uv_buffer_id);
    glDeleteVertexArrays(1, &vao_id);
}

void Particle::renderOGL(Window* window, const TransformSim3& world_from_body)
{
	glBindVertexArray(vao_id);
	
    glUseProgram(program_id);

	if (!tex)
    {
        tex = window->getDefaultTex();
    }

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->getID());
    glUniform1i(tex_sampler_id, 0);

	Mat4 world_from_body_mat = world_from_body.matrix();
	
	Mat4 cam_from_world_mat = window->getCamFromWorldMatrix();
	
    Mat4 MVP = window->getProjectionMatrix();// * cam_from_world_mat * world_from_body_mat;

    glUniformMatrix4fv(mvp_matrix_id, 1, GL_FALSE, MVP.data());
	
	Mat4 cam_from_body_mat = cam_from_world_mat * world_from_body_mat;
    glUniformMatrix4fv(modelview_matrix_id, 1, GL_FALSE, cam_from_body_mat.data());

    glUniform3fv(color_id, 1, color.data());
	glUniform1f(size_id, size * world_from_body.scale);
	glUniform1f(opacity_id, opacity);

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

    glEnableVertexAttribArray(uv_attr_loc);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_id);
    glVertexAttribPointer(
        uv_attr_loc,                                // attribute
        2,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	for (const auto& pt : pts)
	{
		glUniform3fv(offset_id, 1, pt.data());
		glDrawArrays(GL_TRIANGLES, 0, verts.cols());
	}
	
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

    glDisableVertexAttribArray(pos_attr_loc);
    glDisableVertexAttribArray(uv_attr_loc);
}

}