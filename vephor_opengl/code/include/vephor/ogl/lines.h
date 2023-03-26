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
    void renderOGL(Window* window, const TransformSim3& world_from_body);
    void cleanup();
private:
    MatX verts;
    MatX colors;
    bool is_strip = true;

    GLuint vao_id;
    GLuint pos_buffer_id;
    GLuint color_buffer_id;
    GLuint program_id;

    GLuint pos_attr_loc;
    GLuint color_attr_loc;

    GLuint mvp_matrix_id;
};

}