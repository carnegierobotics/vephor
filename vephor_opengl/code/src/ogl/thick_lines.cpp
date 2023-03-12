#include "vephor/ogl/thick_lines.h"

namespace vephor
{


string thickLinesVertexShader = R"(
    #version 430
    layout(location = 0) in vec3 pos_in_model;
	layout(location = 1) in vec3 next_in_model;
	layout(location = 2) in vec3 prev_in_model;
    layout(location = 3) in vec4 color;
	layout(location = 4) in float dir;
    
    uniform mat4 proj_from_model;
	uniform float thickness;
	uniform float aspect;
    uniform int use_miter;
	
	out vec4 col;
    
    void main()
    {
		vec2 aspect_vec = vec2(aspect, 1.0);
		
		vec4 pos_proj = proj_from_model * vec4(pos_in_model, 1.0);
		vec4 next_proj = proj_from_model * vec4(next_in_model, 1.0);
		vec4 prev_proj = proj_from_model * vec4(prev_in_model, 1.0);
		
		vec2 pos_screen = pos_proj.xy / pos_proj.w * aspect_vec;
		vec2 next_screen = next_proj.xy / next_proj.w * aspect_vec;
		vec2 prev_screen = prev_proj.xy / prev_proj.w * aspect_vec;
		
		float len = thickness;

		vec2 dir_vec = vec2(0,0);
		if (pos_screen == prev_screen)
		{
			dir_vec = normalize(next_screen - pos_screen);
		}
		else if (pos_screen == next_screen)
		{
			dir_vec = normalize(pos_screen - prev_screen);
		}
		else
		{
			vec2 dir_0 = normalize(pos_screen - prev_screen);

			if (use_miter == 1)
			{
				vec2 dir_1 = normalize(next_screen - pos_screen);
				vec2 tangent = normalize(dir_0 + dir_1);
				vec2 perp = vec2(-dir_0.y, dir_0.x);
				vec2 miter = vec2(-tangent.y, tangent.x);
				dir_vec = tangent;
				len = thickness / dot(miter, perp);
			}
			else
			{
				dir_vec = dir_0;
			}
		}
		
		vec2 normal = vec2(-dir_vec.y, dir_vec.x);
	    normal *= len;
	    normal.x /= aspect;
		
        gl_Position = pos_proj + vec4(normal * dir, 0, 0) * pos_proj.w;
		
        col = color;
    }
)";

string thickLinesFragmentShader = R"(
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

ThickLines::ThickLines(
    const MatXRef& p_verts, 
    const MatXRef& p_colors,
	const Vec4& p_default_color)
{
	verts.resize(3,p_verts.rows()*2);
	next.resize(3,p_verts.rows()*2);
	prev.resize(3,p_verts.rows()*2);
	colors.resize(4,p_verts.rows()*2);
	dirs.resize(p_verts.rows()*2);
	
	verts.block(0,0,3,p_verts.rows()) = p_verts.transpose();
	verts.block(0,p_verts.rows(),3,p_verts.rows()) = p_verts.transpose();
	
	next.block(0,0,3,p_verts.rows()-1) = p_verts.transpose().block(0,1,3,p_verts.rows()-1);
	next.block(0,p_verts.rows(),3,p_verts.rows()-1) = p_verts.transpose().block(0,1,3,p_verts.rows()-1);
	next.col(p_verts.rows()-1) = p_verts.row(p_verts.rows()-1);
	next.col(p_verts.rows()*2-1) = p_verts.row(p_verts.rows()-1);
	
	prev.col(0) = p_verts.row(0);
	prev.col(p_verts.rows()) = p_verts.row(0);
	prev.block(0,1,3,p_verts.rows()-1) = p_verts.transpose().block(0,0,3,p_verts.rows()-1);
	prev.block(0,p_verts.rows()+1,3,p_verts.rows()-1) = p_verts.transpose().block(0,0,3,p_verts.rows()-1);
	
	colors.block(0,0,4,p_verts.rows()*2).colwise() = p_default_color;
    if (p_colors.cols() == 4 && p_colors.rows() > 0)
	{
        colors.block(0,0,4,p_colors.rows()) = p_colors.transpose();
		colors.block(0,p_verts.rows(),4,p_colors.rows()) = p_colors.transpose();
	}
	
	dirs.segment(0,p_verts.rows()).fill(-1);
	dirs.segment(p_verts.rows(),p_verts.rows()).fill(1);
	
	indices.resize((p_verts.rows()-1)*6);
	
	int ind = 0;
	for (int i = 0; i < p_verts.rows() - 1; i++)
	{
		indices[ind++] = i;
		indices[ind++] = i+p_verts.rows();
		indices[ind++] = i+1+p_verts.rows();
		
		indices[ind++] = i;
		indices[ind++] = i+1+p_verts.rows();
		indices[ind++] = i+1;
	}
	
	
	vert_shader_id = compileShaders(thickLinesVertexShader, GL_VERTEX_SHADER);
    frag_shader_id = compileShaders(thickLinesFragmentShader, GL_FRAGMENT_SHADER);

    program_id = linkProgram(vert_shader_id, frag_shader_id);

	pos_attr_loc = glGetAttribLocation(program_id, "pos_in_model");
	next_attr_loc = glGetAttribLocation(program_id, "next_in_model");
	prev_attr_loc = glGetAttribLocation(program_id, "prev_in_model");
    color_attr_loc = glGetAttribLocation(program_id, "color");
	dir_attr_loc = glGetAttribLocation(program_id, "dir");
	
	mvp_matrix_id = glGetUniformLocation(program_id, "proj_from_model");
	thickness_id = glGetUniformLocation(program_id, "thickness");
	aspect_id = glGetUniformLocation(program_id, "aspect");
	use_miter_id = glGetUniformLocation(program_id, "use_miter");
	
	glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);
	
	glGenBuffers(1, &pos_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, pos_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);
	
	glGenBuffers(1, &next_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, next_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, next.size() * sizeof(GLfloat), next.data(), GL_STATIC_DRAW);
	
	glGenBuffers(1, &prev_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, prev_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, prev.size() * sizeof(GLfloat), prev.data(), GL_STATIC_DRAW);
	
    glGenBuffers(1, &color_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), colors.data(), GL_STATIC_DRAW);
	
	glGenBuffers(1, &dir_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, dir_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, dirs.size() * sizeof(GLfloat), dirs.data(), GL_STATIC_DRAW);
	
	
	glGenBuffers(1, &ibo_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
}

ThickLines::~ThickLines()
{
	glDeleteBuffers(1, &pos_buffer_id);
	glDeleteBuffers(1, &next_buffer_id);
	glDeleteBuffers(1, &prev_buffer_id);
	glDeleteBuffers(1, &color_buffer_id);
	glDeleteBuffers(1, &dir_buffer_id);
	glDeleteBuffers(1, &ibo_id);
    glDeleteVertexArrays(1, &vao_id);
}

void ThickLines::renderOGL(Window* window, const TransformSim3& world_from_body)
{
    glUseProgram(program_id);

    Mat4 world_from_body_mat = world_from_body.matrix();

    Mat4 MVP = window->getProjectionMatrix() * window->getCamFromWorldMatrix() * world_from_body_mat;

    glUniformMatrix4fv(mvp_matrix_id, 1, GL_FALSE, MVP.data());
	glUniform1f(thickness_id, line_width);
	glUniform1f(aspect_id, (float)window->getSize()[0] / (float)window->getSize()[1]);
	glUniform1f(use_miter_id, 1);
	
	glBindVertexArray(vao_id);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, pos_buffer_id);
    glVertexAttribPointer(
        pos_attr_loc,       // attr
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
	
	glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, next_buffer_id);
    glVertexAttribPointer(
        next_attr_loc,      // attr
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
	
	glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, prev_buffer_id);
    glVertexAttribPointer(
        prev_attr_loc,      // attr
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id);
	glVertexAttribPointer(
        color_attr_loc,     // attr
        4,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
	
	glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, dir_buffer_id);
	glVertexAttribPointer(
        dir_attr_loc,     // attr
        1,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
	
	
	glDisable(GL_CULL_FACE);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
	glDrawElements(GL_TRIANGLES, indices.rows(), GL_UNSIGNED_INT, 0);
	
	glEnable(GL_CULL_FACE);
	
	
	glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
}

}