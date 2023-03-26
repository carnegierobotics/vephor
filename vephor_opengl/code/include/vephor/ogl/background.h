#pragma once

#include "window.h"

namespace vephor
{

class Background
{
public:
    Background(
		const shared_ptr<Texture>& tex
    );
    ~Background();
	void setScale(float p_scale){scale = p_scale;}
	float getScale() const {return scale;}
    void renderOGL(Window* window, const TransformSim3& world_from_body);
    void cleanup();
private:
    MatX verts;
	MatX uvs;
    float scale = 1.0f;
    shared_ptr<Texture> tex;

    GLuint vao_id;
    GLuint pos_buffer_id;
	GLuint uv_buffer_id;
    GLuint program_id;

    GLuint pos_attr_loc;
	GLuint uv_attr_loc;

	GLuint tex_sampler_id;
};

}