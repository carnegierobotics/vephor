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

namespace vephor
{

class MaterialBuilder;

struct MaterialState
{
    Vec3 diffuse = Vec3(1,1,1);
    Vec3 ambient = Vec3(1,1,1);
	Vec3 emissive = Vec3(0,0,0);
    float specular = 1.0f;
	float opacity = 1.0f;
    float size = 1.0f;

    shared_ptr<Texture> tex;
	shared_ptr<Texture> normal_map;
    shared_ptr<CubeTexture> cube_tex;
    unordered_map<string, shared_ptr<Texture>> extra_textures;
};

class MaterialProgram
{
public:
    MaterialProgram()
    {
        for (size_t i = 0; i < MAX_NUM_POINT_LIGHTS; i++)
        {
            light_point_pos_id[i] = std::numeric_limits<GLuint>::max();
            light_point_strength_id[i] = std::numeric_limits<GLuint>::max();
        }
    }

    GLuint getPosAttrLoc() const {return pos_attr_loc;}
    GLuint getUVAttrLoc() const {return uv_attr_loc;}
    GLuint getNormAttrLoc() const {return norm_attr_loc;}
    GLuint getTangentAttrLoc() const {return tangent_attr_loc;}
    GLuint getBitangentAttrLoc() const {return bitangent_attr_loc;}
    GLuint getOffsetAttrLoc() const {return offset_attr_loc;}
    GLuint getColorAttrLoc() const {return color_attr_loc;}
    GLuint getSizeAttrLoc() const {return size_attr_loc;}

    void activate(Window* window, const TransformSim3& world_from_body, const MaterialState& state);
    void deactivate();

    string getTag() const {return tag;}
private:
    friend MaterialBuilder;

    static const int MAX_NUM_POINT_LIGHTS = 4;

    string tag;
    GLuint program_id = std::numeric_limits<GLuint>::max();

    GLuint pos_attr_loc = std::numeric_limits<GLuint>::max();
    GLuint uv_attr_loc = std::numeric_limits<GLuint>::max();
    GLuint norm_attr_loc = std::numeric_limits<GLuint>::max();
    GLuint tangent_attr_loc = std::numeric_limits<GLuint>::max();
    GLuint bitangent_attr_loc = std::numeric_limits<GLuint>::max();
    GLuint offset_attr_loc = std::numeric_limits<GLuint>::max();
    GLuint color_attr_loc = std::numeric_limits<GLuint>::max();
    GLuint size_attr_loc = std::numeric_limits<GLuint>::max();
    
    GLuint proj_from_model_matrix_id = std::numeric_limits<GLuint>::max();
    GLuint proj_from_camera_matrix_id = std::numeric_limits<GLuint>::max();
    GLuint camera_from_world_matrix_id = std::numeric_limits<GLuint>::max();
    GLuint world_from_model_matrix_id = std::numeric_limits<GLuint>::max();
    GLuint camera_from_model_matrix_id = std::numeric_limits<GLuint>::max();
    GLuint diffuse_id = std::numeric_limits<GLuint>::max();
    GLuint ambient_id = std::numeric_limits<GLuint>::max();
    GLuint emissive_id = std::numeric_limits<GLuint>::max();
    GLuint specular_id = std::numeric_limits<GLuint>::max();
    GLuint light_ambient_id = std::numeric_limits<GLuint>::max();
    GLuint num_point_lights_id = std::numeric_limits<GLuint>::max();
    GLuint light_point_pos_id[MAX_NUM_POINT_LIGHTS];
    GLuint light_point_strength_id[MAX_NUM_POINT_LIGHTS];
    GLuint dir_light_dir_id = std::numeric_limits<GLuint>::max();
    GLuint dir_light_strength_id = std::numeric_limits<GLuint>::max();
    GLuint dir_light_proj_from_world_id = std::numeric_limits<GLuint>::max();
    GLuint dir_light_shadow_map_sampler_id = std::numeric_limits<GLuint>::max();
    GLuint opacity_id = std::numeric_limits<GLuint>::max();
    GLuint tex_sampler_id = std::numeric_limits<GLuint>::max();
    GLuint normal_sampler_id = std::numeric_limits<GLuint>::max();
    GLuint cube_tex_sampler_id = std::numeric_limits<GLuint>::max();
    GLuint time_id = std::numeric_limits<GLuint>::max();
    GLuint size_id = std::numeric_limits<GLuint>::max();
    GLuint aspect_id = std::numeric_limits<GLuint>::max();
    GLuint screen_size_id = std::numeric_limits<GLuint>::max();

    unordered_map<string, GLuint> extra_texture_ids;

    bool infinite_depth = false;
};

using MaterialProgramSet = unordered_map<string, shared_ptr<MaterialProgram>>;

struct Material
{
    MaterialProgramSet set;
    MaterialState state;

    Material(){}
    Material(const MaterialProgramSet& p_set)
    : set(p_set)
    {}
};

class MaterialBuilder
{
public:
    bool tex = false;
    bool normal_map = false;
    bool cube_tex = false;
    bool dir_light = false;
    bool point_lights = false;
    bool time = false;
    bool vertex_color = false;
    bool materials = true;
    bool offset = false;
    bool uniform_size = false;
    bool instanced_size = false;
    bool billboard = false;
    bool screen_space = false;
    bool infinite_depth = false;
    bool screen_space_tex_coords = false;
    bool no_color = false;
    bool dir_light_shadows = false;
    bool vert_world_from_model = false;
    bool frag_cam_from_world = false;
    bool frag_proj_from_camera = false;

    unordered_map<string, vector<string>> extra_sections;
    vector<string> extra_textures;

    std::string produceVertexShader() const;
    std::string produceFragmentShader() const;
    std::string getTag() const;
    void saveShaders() const;
    std::shared_ptr<MaterialProgram> build() const;
    MaterialProgramSet buildSet(bool simple_depth=true) const;
};

}