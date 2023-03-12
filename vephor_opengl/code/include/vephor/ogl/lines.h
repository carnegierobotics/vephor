#pragma once

#include "window.h"

namespace vephor
{

class Lines
{
public:
    Lines(
        const MatXRef& line_verts,
        const MatXRef& colors = MatX(),
		const Color& default_color = Vec4(1,1,1,1)
    );
    ~Lines();
	void setLineStrip(bool p_is_strip){is_strip = p_is_strip;}
    //void setLineWidth(float p_line_width){line_width = p_line_width;}
    void renderOGL(Window* window, const TransformSim3& world_from_body);
private:
    MatX verts;
    MatX colors;
    bool is_strip = true;

    //float line_width = 1.0f;

    GLuint vao_id;
    GLuint pos_buffer_id;
    GLuint color_buffer_id;
    GLuint vert_shader_id;
    GLuint frag_shader_id;
    GLuint program_id;

    GLuint pos_attr_loc;
    GLuint color_attr_loc;

    GLuint mvp_matrix_id;
};

}