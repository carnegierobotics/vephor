#pragma once

#include "window.h"

namespace vephor
{

class Particle
{
public:
    Particle(
        const vector<Vec3>& pts
    );
    ~Particle();
	void setTexture(const shared_ptr<Texture>& p_tex){tex = p_tex;}
	void setColor(const Vec3& p_color){color = p_color;}
	void setSize(float p_size){size = p_size;}
	void setOpacity(const float& p_opacity){opacity = p_opacity;}
    void renderOGL(Window* window, const TransformSim3& world_from_body);
	void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node);
	void onRemoveFromWindow(Window* window);
private:
    MatX verts;
	MatX uvs;
	Vec3 color = Vec3(1,1,1);
	float size = 0.03f;
	float opacity = 1.0f;
    shared_ptr<Texture> tex;
    vector<Vec3> pts;

    GLuint vao_id;
    GLuint pos_buffer_id;
	GLuint uv_buffer_id;
    GLuint program_id;

    GLuint pos_attr_loc;
	GLuint uv_attr_loc;
	
	GLuint mvp_matrix_id;
    GLuint modelview_matrix_id;
	GLuint size_id;
    GLuint color_id;
	GLuint opacity_id;
	GLuint offset_id;
	GLuint tex_sampler_id;
};

}