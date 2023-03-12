#pragma once

#include "window.h"

namespace vephor
{

class InstancedPoints
{
public:
    InstancedPoints(
        const MatXRef& pts,
        const MatXRef& colors = MatX(),
		const Vec4& default_color = Vec4(1,1,1,1)
    );
    ~InstancedPoints();
	void setTexture(const shared_ptr<Texture>& p_tex){tex = p_tex;}
	void setSize(float p_size){size = p_size;}
	void setOpacity(const float& p_opacity){opacity = p_opacity;}
	void setScreenSpaceMode(bool p_ss_mode)
	{
		ss_mode = p_ss_mode;
	}
    void renderOGL(Window* window, const TransformSim3& world_from_body);
private:
    MatX verts;
	MatX uvs;
	MatX offsets;
	MatX colors;
	float size = 0.03f;
	float opacity = 1.0f;
	bool ss_mode = false;
    shared_ptr<Texture> tex;

    GLuint vao_id;
    GLuint pos_buffer_id;
	GLuint uv_buffer_id;
	GLuint offset_buffer_id;
	GLuint color_buffer_id;
    GLuint program_id;
	GLuint ss_program_id;

    GLuint pos_attr_loc;
	GLuint uv_attr_loc;
	GLuint offset_attr_loc;
	GLuint color_attr_loc;
	
	GLuint mvp_matrix_id;
    GLuint modelview_matrix_id;
	GLuint size_id;
	GLuint tex_sampler_id;
	
	GLuint ss_pos_attr_loc;
	GLuint ss_uv_attr_loc;
	GLuint ss_offset_attr_loc;
	GLuint ss_color_attr_loc;
	
	GLuint ss_mvp_matrix_id;
    GLuint ss_modelview_matrix_id;
	GLuint ss_size_id;
	GLuint ss_aspect_id;
	GLuint ss_tex_sampler_id;
};

}