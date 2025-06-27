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

namespace vephor
{

class MaterialBuilder;

class Material
{
public:
    Material()
    {
        for (size_t i = 0; i < MAX_NUM_POINT_LIGHTS; i++)
        {
            light_point_pos_id[i] = std::numeric_limits<GLuint>::max();
            light_point_strength_id[i] = std::numeric_limits<GLuint>::max();
        }
    }
    void activate();
    void deactivate();
private:
    friend MaterialBuilder;

    static const int MAX_NUM_POINT_LIGHTS = 4;

    GLuint program_id = std::numeric_limits<GLuint>::max();

    Vec3 diffuse;
    Vec3 ambient;
	Vec3 emissive;
    float specular = 1.0f;
	float opacity = 1.0f;

    shared_ptr<Texture> tex;
	shared_ptr<Texture> normal_map;

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
};

class MaterialBuilder
{
public:
    bool tex = false;
    bool normal_map = false;
    bool dir_light = false;
    bool point_lights = false;

    std::string produceVertexShader() const;
    std::string produceFragmentShader() const;
    std::shared_ptr<Material> build() const;
};

}