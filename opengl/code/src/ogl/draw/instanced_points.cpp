/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

#include "vephor/ogl/draw/instanced_points.h"

namespace vephor
{

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

	{
		MaterialBuilder builder;
		builder.tex = true;
		builder.normal_map = false;
		builder.dir_light = false;
		builder.point_lights = false;
		builder.vertex_color = true;
		builder.materials = false;
		builder.offset = true;
		builder.uniform_size = true;
		builder.billboard = true;
		material = builder.build();
	}
}

InstancedPoints::~InstancedPoints()
{
}

void InstancedPoints::onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
{
	curr_window = window;

	createOpenGLBufferForMatX(pos_buffer_id, verts);
    createOpenGLBufferForMatX(uv_buffer_id, uvs);
    createOpenGLBufferForMatX(offset_buffer_id, offsets);
    createOpenGLBufferForMatX(color_buffer_id, colors);

	if (sizes.rows() > 0)
	{
		createOpenGLBufferForMatX(size_buffer_id, sizes);
	}

	setupVAO();
}

void InstancedPoints::onRemoveFromWindow(Window*)
{
	glDeleteBuffers(1, &pos_buffer_id);
    glDeleteBuffers(1, &uv_buffer_id);
	glDeleteBuffers(1, &offset_buffer_id);
	glDeleteBuffers(1, &color_buffer_id);
	if (size_buffer_id != std::numeric_limits<GLuint>::max())
		glDeleteBuffers(1, &size_buffer_id);
    glDeleteVertexArrays(1, &vao_id);
}

void InstancedPoints::generateMaterial()
{
	MaterialBuilder builder;
	builder.tex = true;
	builder.normal_map = false;
	builder.dir_light = false;
	builder.point_lights = false;
	builder.vertex_color = true;
	builder.materials = false;
	builder.offset = true;
	if (sizes.rows() > 0)
		builder.instanced_size = true;
	else
		builder.uniform_size = true;
	builder.billboard = true;
	builder.screen_space = ss_mode;
	material = builder.build();

	setupVAO();
}

void InstancedPoints::setupVAO()
{
	if (!curr_window)
        return;

	if (vao_id != std::numeric_limits<GLuint>::max())
        glDeleteVertexArrays(1, &vao_id);
	
	glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    addOpenGLBufferToActiveVAO(pos_buffer_id, material->getPosAttrLoc(), 3);
    addOpenGLBufferToActiveVAO(uv_buffer_id, material->getUVAttrLoc(), 2);
    addOpenGLBufferToActiveVAO(offset_buffer_id, material->getOffsetAttrLoc(), 3);
	glVertexAttribDivisor(material->getOffsetAttrLoc(), 1);
    addOpenGLBufferToActiveVAO(color_buffer_id, material->getColorAttrLoc(), 4);
	glVertexAttribDivisor(material->getColorAttrLoc(), 1);
	if (sizes.rows() > 0)
	{
		addOpenGLBufferToActiveVAO(size_buffer_id, material->getSizeAttrLoc(), 1);
		glVertexAttribDivisor(material->getSizeAttrLoc(), 1);
	}

    glBindVertexArray(0);
}

void InstancedPoints::renderOGL(Window* window, const TransformSim3& world_from_body)
{
	material_state.size = size * world_from_body.scale;
	material->activate(window, world_from_body, material_state);
	

	glBindVertexArray(vao_id);


	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glDrawArraysInstanced(GL_TRIANGLES, 0, verts.cols(), offsets.cols());
	
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	glBindVertexArray(0);

	material->deactivate();
}

}