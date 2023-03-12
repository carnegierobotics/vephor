#pragma once

#include "window.h"

namespace vephor
{

class ThickLines
{
public:
    ThickLines(
        const MatXRef& line_verts,
        const MatXRef& colors = MatX(),
		const Vec4& default_color = Vec4(1,1,1,1)
    );
    ~ThickLines();
    void setLineWidth(float p_line_width){line_width = p_line_width;}
    void renderOGL(Window* window, const TransformSim3& world_from_body);
private:
	MatX verts;
	MatX next;
	MatX prev;
    MatX colors;
	VecX dirs;
	VecXui indices;
    float line_width = 0.1f;

    GLuint vert_shader_id;
    GLuint frag_shader_id;
    GLuint program_id;
	
	GLuint vao_id;
	GLuint pos_buffer_id;
	GLuint next_buffer_id;
	GLuint prev_buffer_id;
    GLuint color_buffer_id;
	GLuint dir_buffer_id;
	GLuint ibo_id;
	
	GLuint pos_attr_loc;
	GLuint next_attr_loc;
	GLuint prev_attr_loc;
    GLuint color_attr_loc;
	GLuint dir_attr_loc;

    GLuint mvp_matrix_id;
	GLuint thickness_id;
	GLuint aspect_id;
	GLuint use_miter_id;
};

}