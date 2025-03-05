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

namespace vephor
{

class Mesh
{
public:
    Mesh(
        const MeshData& data, 
        const Color& color = Color(1,1,1),
        float diffuse_strength = 1.0,
        float ambient_strength = 1.0,
		float emissive_strength = 0.0);
    ~Mesh();
	void setTexture(const shared_ptr<Texture>& p_tex){tex = p_tex;}
	void setNormalMap(const shared_ptr<Texture>& p_normal_map){normal_map = p_normal_map;}
	void setDiffuse(const Color& p_color){diffuse = p_color.getRGB();}
	Color getDiffuse() const {return diffuse;}
	void setAmbient(const Color& p_color){ambient = p_color.getRGB();}
	Color getAmbient() const {return ambient;}
    void setEmissive(const Color& p_color){emissive = p_color.getRGB();}
	Color getEmissive() const {return emissive;}
	void setOpacity(const float& p_opacity){opacity = p_opacity;}
	void setCull(bool p_cull){cull = p_cull;}
    void setSpecular(bool p_specular){specular = p_specular;}
    void setCustomShader(
        const string& tag, 
        const string& vertex, 
        const string& fragment, 
        const string& normal_map_vertex, 
        const string& normal_map_fragment)
    {
        custom_tag = +"_" + tag;
        custom_vertex = vertex;
        custom_fragment = fragment;
        custom_normal_map_vertex = normal_map_vertex;
        custom_normal_map_fragment = normal_map_fragment;
    }
    void renderOGL(Window* window, const TransformSim3& world_from_body);
    void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node);
	void onRemoveFromWindow(Window* window);
private:
    static const int MAX_NUM_POINT_LIGHTS = 4;

    struct MeshSettings
    {
        GLuint vao_id = std::numeric_limits<GLuint>::max();
        GLuint program_id = std::numeric_limits<GLuint>::max();

        GLuint pos_attr_loc = std::numeric_limits<GLuint>::max();
        GLuint uv_attr_loc = std::numeric_limits<GLuint>::max();
        GLuint norm_attr_loc = std::numeric_limits<GLuint>::max();
        GLuint tangent_attr_loc = std::numeric_limits<GLuint>::max();
        GLuint bitangent_attr_loc = std::numeric_limits<GLuint>::max();
        
        GLuint mvp_matrix_id = std::numeric_limits<GLuint>::max();
        GLuint view_matrix_id = std::numeric_limits<GLuint>::max();
        GLuint model_matrix_id = std::numeric_limits<GLuint>::max();
        GLuint modelview_matrix_id = std::numeric_limits<GLuint>::max();
        GLuint diffuse_id = std::numeric_limits<GLuint>::max();
        GLuint ambient_id = std::numeric_limits<GLuint>::max();
        GLuint emissive_id = std::numeric_limits<GLuint>::max();
        GLuint specular_id = std::numeric_limits<GLuint>::max();
        GLuint light_ambient_id = std::numeric_limits<GLuint>::max();
        GLuint num_point_lights_id = std::numeric_limits<GLuint>::max();
        GLuint light_point_pos_id[MAX_NUM_POINT_LIGHTS];
        GLuint light_point_strength_id[MAX_NUM_POINT_LIGHTS];
        GLuint light_dir_id = std::numeric_limits<GLuint>::max();
        GLuint light_dir_strength_id = std::numeric_limits<GLuint>::max();
        GLuint opacity_id = std::numeric_limits<GLuint>::max();
        GLuint tex_sampler_id = std::numeric_limits<GLuint>::max();
        GLuint normal_sampler_id = std::numeric_limits<GLuint>::max();

        MeshSettings()
        {
            for (size_t i = 0; i < MAX_NUM_POINT_LIGHTS; i++)
            {
                light_point_pos_id[i] = std::numeric_limits<GLuint>::max();
                light_point_strength_id[i] = std::numeric_limits<GLuint>::max();
            }
        }
    };

    void fillInCommonProps(MeshSettings& settings);
    void renderOGLForSettings(Window* window, const TransformSim3& world_from_body, MeshSettings& settings);

    Window* curr_window = NULL;
    size_t curr_window_count = 0;

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
    Vec3 diffuse;
    Vec3 ambient;
	Vec3 emissive;
    float specular = 1.0f;
	float opacity = 1.0f;
    shared_ptr<Texture> tex;
	shared_ptr<Texture> normal_map;

    bool cull = true;
	
    MeshSettings standard;
    MeshSettings no_normal_map;

    string custom_tag;
    string custom_vertex;
    string custom_fragment;
    string custom_normal_map_vertex;
    string custom_normal_map_fragment;
};

// Allows render parameters of a mesh to be changed without needing to store the memory of the mesh multiple times
class WrappedMesh
{
public:
    WrappedMesh(
        const shared_ptr<Mesh>& mesh,
        const Color& color = Color(1,1,1),
        float diffuse_strength = 1.0,
        float ambient_strength = 1.0,
		float emissive_strength = 0.0
    );
    void renderOGL(Window* window, const TransformSim3& world_from_body);
    void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node);
	void onRemoveFromWindow(Window* window);
private:
    shared_ptr<Mesh> mesh;
    Color color;
    float diffuse_strength;
    float ambient_strength;
    float emissive_strength;
};

}