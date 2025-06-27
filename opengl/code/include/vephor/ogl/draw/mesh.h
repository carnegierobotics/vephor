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
#include "../io.h"
#include "material.h"

namespace vephor
{

class Mesh
{
public:
    Mesh(
        const MeshData& data, 
        const shared_ptr<Material>& material
    );
    Mesh(
        const MeshData& data, 
        const Color& color = Color(1,1,1),
        float diffuse_strength = 1.0,
        float ambient_strength = 1.0,
        float emissive_strength = 0.0
    );
    ~Mesh();
	void setCull(bool p_cull){cull = p_cull;}

    void setMaterial(const shared_ptr<Material>& p_material);
    shared_ptr<Material> getMaterial() {return material;}

    void setDiffuse(const Color& p_color){material->setDiffuse(p_color);}
	Color getDiffuse() const {return material->getDiffuse();}
	void setAmbient(const Color& p_color){material->setAmbient(p_color);}
	Color getAmbient() const {return material->getAmbient();}
    void setEmissive(const Color& p_color){material->setEmissive(p_color);}
	Color getEmissive() const {return material->getEmissive();}
    void setSpecular(bool p_specular){material->setSpecular(p_specular);}
	void setOpacity(const float& p_opacity){material->setOpacity(p_opacity);}
    void setTexture(const shared_ptr<Texture>& p_tex){material->setTexture(p_tex);}
	void setNormalMap(const shared_ptr<Texture>& p_normal_map){material->setTexture(p_normal_map);}

    void renderOGL(Window* window, const TransformSim3& world_from_body);
    void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node);
	void onRemoveFromWindow(Window* window);
private:
    void setupVAO();

    Window* curr_window = NULL;
    size_t curr_window_count = 0;

    GLuint vao_id = std::numeric_limits<GLuint>::max();

    GLuint pos_buffer_id = std::numeric_limits<GLuint>::max();
    GLuint uv_buffer_id = std::numeric_limits<GLuint>::max();
    GLuint norm_buffer_id = std::numeric_limits<GLuint>::max();
    GLuint tangent_buffer_id = std::numeric_limits<GLuint>::max();
    GLuint bitangent_buffer_id = std::numeric_limits<GLuint>::max();

    MatX verts;
    MatX uvs;
    MatX norms;
	MatX tangents;
	MatX bitangents;

    bool cull = true;

    shared_ptr<Material> material;
};

// Allows render parameters of a mesh to be changed without needing to store the memory of the mesh multiple times
class WrappedMesh
{
public:
    WrappedMesh(
        const shared_ptr<Mesh>& mesh,
        const shared_ptr<Material>& material
    );
    void renderOGL(Window* window, const TransformSim3& world_from_body);
    void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node);
	void onRemoveFromWindow(Window* window);
private:
    shared_ptr<Mesh> mesh;
    shared_ptr<Material> material;
};

}