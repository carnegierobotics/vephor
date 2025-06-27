/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

#include "vephor/ogl/draw/mesh.h"

namespace vephor
{

Mesh::Mesh(
    const MeshData& p_data,
    const shared_ptr<Material>& p_material
)
: verts(p_data.verts.transpose()), 
uvs(p_data.uvs.transpose()), 
norms(p_data.norms.transpose()),
material(p_material)
{
    if (verts.cols() % 3 != 0)
    {
        throw std::runtime_error("Number of verts for Mesh must be a multiple of 3.");
    }

    if (uvs.cols() % 3 != 0)
    {
        throw std::runtime_error("Number of uvs for Mesh must be a multiple of 3.");
    }

    if (norms.cols() % 3 != 0)
    {
        throw std::runtime_error("Number of norms for Mesh must be a multiple of 3.");
    }

    if (uvs.cols() == 0)
    {
        uvs.resize(2, verts.cols());
        uvs.setZero();
    }
	
	MatX pre_tangents;
	MatX pre_bitangents;
	computeTangents(p_data, pre_tangents, pre_bitangents);
	
	tangents = pre_tangents.transpose();
	bitangents = pre_bitangents.transpose();
}

Mesh::Mesh(
    const MeshData& p_data, 
    const Color& p_color,
    float p_diffuse_strength,
    float p_ambient_strength,
    float p_emissive_strength
)
: verts(p_data.verts.transpose()), 
uvs(p_data.uvs.transpose()), 
norms(p_data.norms.transpose())
{
    if (verts.cols() % 3 != 0)
    {
        throw std::runtime_error("Number of verts for Mesh must be a multiple of 3.");
    }

    if (uvs.cols() % 3 != 0)
    {
        throw std::runtime_error("Number of uvs for Mesh must be a multiple of 3.");
    }

    if (norms.cols() % 3 != 0)
    {
        throw std::runtime_error("Number of norms for Mesh must be a multiple of 3.");
    }

    if (uvs.cols() == 0)
    {
        uvs.resize(2, verts.cols());
        uvs.setZero();
    }
	
	MatX pre_tangents;
	MatX pre_bitangents;
	computeTangents(p_data, pre_tangents, pre_bitangents);
	
	tangents = pre_tangents.transpose();
	bitangents = pre_bitangents.transpose();

    MaterialBuilder builder;
    builder.dir_light = true;
    builder.point_lights = true;
    builder.tex = true;
    builder.normal_map = false;
    material = builder.build();

    material->setDiffuse(Color((Vec3)(p_color.getRGB()*p_diffuse_strength)));
    material->setAmbient(Color((Vec3)(p_color.getRGB()*p_ambient_strength)));
    material->setEmissive(Color((Vec3)(p_color.getRGB()*p_emissive_strength)));
    material->setOpacity(p_color.getAlpha());
}

Mesh::~Mesh()
{
}

void Mesh::setMaterial(const shared_ptr<Material>& p_material)
{
    material = p_material;
    setupVAO();
}

void createOpenGLBufferForMatX(GLuint& buffer_id, const MatX& mat)
{
    glGenBuffers(1, &buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
	glBufferData(GL_ARRAY_BUFFER, mat.size() * sizeof(GLfloat), mat.data(), GL_STATIC_DRAW);
}

void addOpenGLBufferToActiveVAO(
    GLuint buffer_id,
    GLuint attr_loc,
    int size
)
{
    if (attr_loc == std::numeric_limits<GLuint>::max())
        return;

    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glVertexAttribPointer(
        attr_loc,                         // attribute
        size,                             // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    glEnableVertexAttribArray(attr_loc);
}

void Mesh::setupVAO()
{
    if (!curr_window)
        return;

    if (vao_id != std::numeric_limits<GLuint>::max())
        glDeleteVertexArrays(1, &vao_id);

    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    addOpenGLBufferToActiveVAO(pos_buffer_id, material->getPosAttrLoc(), 3);
    addOpenGLBufferToActiveVAO(uv_buffer_id, material->getUVAttrLoc(), 2);
    addOpenGLBufferToActiveVAO(norm_buffer_id, material->getNormAttrLoc(), 3);
    addOpenGLBufferToActiveVAO(tangent_buffer_id, material->getTangentAttrLoc(), 3);
    addOpenGLBufferToActiveVAO(bitangent_buffer_id, material->getBitangentAttrLoc(), 3);

    glBindVertexArray(0);
}

void Mesh::onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
{	
    curr_window_count++;

    if (curr_window == window)
        return;

    if (curr_window != NULL)
        throw std::runtime_error("A single drawing object can't be added to multiple windows.");

    curr_window = window;

    createOpenGLBufferForMatX(pos_buffer_id, verts);
    createOpenGLBufferForMatX(uv_buffer_id, verts);
    createOpenGLBufferForMatX(norm_buffer_id, uvs);
    createOpenGLBufferForMatX(tangent_buffer_id, tangents);
    createOpenGLBufferForMatX(bitangent_buffer_id, bitangents);
        
    setupVAO();
}

void Mesh::onRemoveFromWindow(Window*)
{
    if (curr_window_count == 0)
        return;

    curr_window_count--;

    if (curr_window_count == 0)
    {
        glDeleteBuffers(1, &pos_buffer_id);
        glDeleteBuffers(1, &uv_buffer_id);
        glDeleteBuffers(1, &norm_buffer_id);
        glDeleteBuffers(1, &tangent_buffer_id);
        glDeleteBuffers(1, &bitangent_buffer_id);
        glDeleteVertexArrays(1, &vao_id);

        curr_window = NULL;
    }
}

void Mesh::renderOGL(Window* window, const TransformSim3& world_from_body)
{
    if (!material)
        return;

    material->activate(window, world_from_body);

	glBindVertexArray(vao_id);
	
	if (!cull)
		glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);

    glDrawArrays(GL_TRIANGLES, 0, verts.cols());

	if (!cull)
		glEnable(GL_CULL_FACE);
	
	glDisable(GL_BLEND);

    glBindVertexArray(0);

    material->deactivate();
}

WrappedMesh::WrappedMesh(
    const shared_ptr<Mesh>& p_mesh,
    const shared_ptr<Material>& p_material
)
: mesh(p_mesh), material(p_material)
{
}

void WrappedMesh::renderOGL(Window* window, const TransformSim3& world_from_body)
{
    mesh->setMaterial(material);
    mesh->renderOGL(window, world_from_body);
}

void WrappedMesh::onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
{
    mesh->onAddToWindow(window, node);
}

void WrappedMesh::onRemoveFromWindow(Window* window)
{
    mesh->onRemoveFromWindow(window);
}

}