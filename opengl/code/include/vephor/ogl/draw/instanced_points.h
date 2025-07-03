/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

#pragma once

#include "../window.h"
#include "material.h"

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
	void setTexture(const shared_ptr<Texture>& p_tex)
	{
		material_state.tex = p_tex;
	}
	void setSize(float p_size)
	{
		size = p_size;
	}
	void setSizes(const MatXRef& p_sizes)
	{
		sizes = p_sizes.transpose();

		if (!curr_window)
		{
			generateMaterial();
			return;
		}

		if (size_buffer_id != std::numeric_limits<GLuint>::max())
			throw std::runtime_error("InstancedPoints size buffer already set.");

		createOpenGLBufferForMatX(size_buffer_id, sizes);

		generateMaterial();
	}
	void setOpacity(const float& p_opacity)
	{
		material_state.opacity = p_opacity;
	}
	void setScreenSpaceMode(bool p_ss_mode)
	{
		ss_mode = p_ss_mode;
		generateMaterial();
	}
    void renderOGL(Window* window, const TransformSim3& world_from_body);
	void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node);
	void onRemoveFromWindow(Window* window);
private:
	void generateMaterial();
	void setupVAO();

	Window* curr_window = NULL;

    MatX verts;
	MatX uvs;
	MatX offsets;
	MatX colors;
	MatX sizes;
	float size = 0.03f;
	bool ss_mode = false;

	shared_ptr<MaterialProgram> material;
	MaterialState material_state;

	GLuint vao_id = std::numeric_limits<GLuint>::max();

    GLuint pos_buffer_id = std::numeric_limits<GLuint>::max();
	GLuint uv_buffer_id = std::numeric_limits<GLuint>::max();
	GLuint offset_buffer_id = std::numeric_limits<GLuint>::max();
	GLuint color_buffer_id = std::numeric_limits<GLuint>::max();
	GLuint size_buffer_id = std::numeric_limits<GLuint>::max();
};

}