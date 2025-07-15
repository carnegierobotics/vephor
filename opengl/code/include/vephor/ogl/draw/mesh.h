/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
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
        const Material& material
    );
    Mesh(
        const MeshData& data, 
        const Color& color = Color(1.0,1.0,1.0),
        float diffuse_strength = 1.0,
        float ambient_strength = 1.0,
        float emissive_strength = 0.0
    );
    ~Mesh();
	void setCull(bool p_cull){cull = p_cull;}

    void setMaterial(const Material& p_material);
    Material getMaterial() {return material;}
    void setMaterialState(const MaterialState& state){material.state = state;}

    void setDiffuse(const Color& p_color){material.state.diffuse = p_color.getRGB();}
	Color getDiffuse() const {return material.state.diffuse;}
	void setAmbient(const Color& p_color){material.state.ambient = p_color.getRGB();}
	Color getAmbient() const {return material.state.ambient;}
    void setEmissive(const Color& p_color){material.state.emissive = p_color.getRGB();}
	Color getEmissive() const {return material.state.emissive;}
    void setSpecular(bool p_specular){material.state.specular = p_specular;}
	void setOpacity(const float& p_opacity){material.state.opacity = p_opacity;}
    void setTexture(const shared_ptr<Texture>& p_tex){material.state.tex = p_tex;}
	void setNormalMap(const shared_ptr<Texture>& p_normal_map){material.state.normal_map = p_normal_map;}
    void setCubeTexture(const shared_ptr<CubeTexture>& p_cube_texture){material.state.cube_tex = p_cube_texture;}

    void renderOGL(Window* window, const TransformSim3& world_from_body);
    void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node);
	void onRemoveFromWindow(Window* window);
private:
    GLuint setupVAO(const shared_ptr<MaterialProgram>& curr_material);

    Window* curr_window = NULL;
    size_t curr_window_count = 0;

    unordered_map<string, GLuint> vaos_for_materials;

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

    Material material;
};

// Allows render parameters of a mesh to be changed without needing to store the memory of the mesh multiple times
class WrappedMesh
{
public:
    WrappedMesh(
        const shared_ptr<Mesh>& mesh,
        const Material& material
    );
    void renderOGL(Window* window, const TransformSim3& world_from_body);
    void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node);
	void onRemoveFromWindow(Window* window);
private:
    shared_ptr<Mesh> mesh;
    Material material;
};

}