/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

#include "vephor/ogl/draw/material.h"

namespace vephor
{

void Material::activate(Window* window, const TransformSim3& world_from_body)
{
    glUseProgram(program_id);

    Mat4 world_from_body_mat = world_from_body.matrix();

	Mat4 MV = window->getCamFromWorldMatrix() * world_from_body_mat;
    Mat4 MVP = window->getProjectionMatrix() * MV;

    glUniformMatrix4fv(mvp_matrix_id, 1, GL_FALSE, MVP.data());
    glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, world_from_body_mat.data());
    glUniformMatrix4fv(view_matrix_id, 1, GL_FALSE, window->getCamFromWorldMatrix().data());
	glUniformMatrix4fv(modelview_matrix_id, 1, GL_FALSE, MV.data());

    glUniform3fv(diffuse_id, 1, diffuse.data());
    glUniform3fv(ambient_id, 1, ambient.data());
	glUniform3fv(emissive_id, 1, emissive.data());
    glUniform1f(specular_id, specular);
    glUniform1f(opacity_id, opacity);

	Vec3 ambient_light_strength = window->getAmbientLight();
	glUniform3fv(light_ambient_id, 1, ambient_light_strength.data());

    if (tex_sampler_id != std::numeric_limits<GLuint>::max())
    {
        if (!tex)
        {
            tex = window->getDefaultTex();
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex->getID());
        glUniform1i(tex_sampler_id, 0);
    }
	
    if (normal_sampler_id != std::numeric_limits<GLuint>::max())
    {
        if (!normal_map)
	    {
            throw std::runtime_error("No normal map found in material.");
        }

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normal_map->getID());
        glUniform1i(normal_sampler_id, 1);
    }

    if (light_dir_id != std::numeric_limits<GLuint>::max())
    {
        auto dir_light = window->getDirLight();

        glUniform3fv(light_dir_id, 1, dir_light.pos.data());
        glUniform1f(light_dir_strength_id, dir_light.strength);
    }

    if (num_point_lights_id != std::numeric_limits<GLuint>::max())
    {
        int num_point_lights = std::max((int)window->getPointLights().size(), (int)MAX_NUM_POINT_LIGHTS);
        glUniform1i(num_point_lights_id, num_point_lights);

        size_t point_light_id = 0;
        for (const auto& point_light : window->getPointLights())
        {
            glUniform3fv(light_point_pos_id[point_light_id], 1, point_light.second.pos.data());
            glUniform1f(light_point_strength_id[point_light_id], point_light.second.strength);
            point_light_id++;

            if (point_light_id >= num_point_lights)
                break;
        }
    }

    if (time_id != std::numeric_limits<GLuint>::max())
    {
        glUniform1f(time_id, window->getCanonicalTime());
    }
}

void Material::deactivate()
{
}

string vertexShaderHeader = R"(
#version 430
)";

string vertexShaderPointLightHeader = R"(
#define MAX_POINT_LIGHTS 4
)";

string vertexShaderLayout = R"(

layout(location = 0) in vec3 pos_in_model;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 normal_in_model;
)";

string vertexShaderNormalMapLayout = R"(
layout(location = 3) in vec3 tangent_in_model;
layout(location = 4) in vec3 bitangent_in_model;
)";

string vertexShaderOut = R"(

out vec3 pos_in_world;
out vec3 normal_in_camera;
out vec3 eye_dir_in_camera;
out vec2 uv;
)";

string vertexShaderNormalMapOut = R"(
out vec3 tangent_in_camera;
out vec3 bitangent_in_camera;
)";

string vertexShaderDirLightOut = R"(
out vec3 dir_light_dir_in_camera;
)";

string vertexShaderPointLightOut = R"(
out vec3 point_light_dir_in_camera[MAX_POINT_LIGHTS];
)";

string vertexShaderUniforms = R"(

uniform mat4 proj_from_model;
uniform mat4 cam_from_world;
uniform mat4 world_from_model;
uniform mat4 cam_from_model;
)";

string vertexShaderDirLightUniforms = R"(
uniform vec3 dir_light_dir_in_world;
)";

string vertexShaderPointLightUniforms = R"(
uniform vec3 point_light_pos_in_world[MAX_POINT_LIGHTS];
uniform int num_point_lights;
)";

string vertexShaderTimeUniforms = R"(
uniform float time;
)";

string vertexShaderMainHeader = R"(

void main()
{
    vec3 curr_pos_in_model = pos_in_model;
    vec3 curr_normal_in_model = normal_in_model;
)";

string vertexShaderMainBody = R"(
    gl_Position = proj_from_model * vec4(curr_pos_in_model, 1);
    
    // Position of the vertex, in worldspace : M * position
    pos_in_world = (world_from_model * vec4(curr_pos_in_model,1)).xyz;
    
    // Vector that goes from the vertex to the camera, in camera space.
    // In camera space, the camera is at the origin (0,0,0).
    vec3 pos_in_camera = (cam_from_world * vec4(pos_in_world,1)).xyz;
    eye_dir_in_camera = vec3(0,0,0) - pos_in_camera;

    // Normal of the the vertex, in camera space
    // Only correct if cam_from_model does not scale the model ! Use its inverse transpose if not.
    normal_in_camera = (cam_from_model * vec4(curr_normal_in_model,0)).xyz; 

    uv = in_uv;
)";

string vertexShaderNormalMapMain = R"(
    tangent_in_camera = (cam_from_model * vec4(tangent_in_model,0)).xyz; 
    bitangent_in_camera = (cam_from_model * vec4(bitangent_in_model,0)).xyz; 
)";

string vertexShaderDirLightMain = R"(
    dir_light_dir_in_camera = ( cam_from_world * vec4(dir_light_dir_in_world,0) ).xyz;
)";

string vertexShaderPointLightMain = R"(
    for (int i = 0; i < num_point_lights; i++)
    {
        // Vector that goes from the vertex to the light, in camera space.
        vec3 point_light_pos_in_camera = ( cam_from_world * vec4(point_light_pos_in_world[i],1)).xyz;
        point_light_dir_in_camera[i] = point_light_pos_in_camera + eye_dir_in_camera;
    }
)";

string vertexShaderMainFooter = R"(
}
)";

std::string MaterialBuilder::produceVertexShader() const
{
    string shader = vertexShaderHeader;

    if (point_lights)
        shader += vertexShaderPointLightHeader;

    shader += vertexShaderLayout;

    if (normal_map)
    {
        shader += vertexShaderNormalMapLayout;
    }

    shader += vertexShaderOut;

    if (normal_map)
    {
        shader += vertexShaderNormalMapOut;
    }

    if (dir_light)
    {
        shader += vertexShaderDirLightOut;
    }

    if (point_lights)
    {
        shader += vertexShaderPointLightOut;
    }

    shader += vertexShaderUniforms;

    if (dir_light)
    {
        shader += vertexShaderDirLightUniforms;
    }

    if (point_lights)
    {
        shader += vertexShaderPointLightUniforms;
    }

    if (time)
    {
        shader += vertexShaderTimeUniforms;
    }

    if (find(extra_sections, string("vertex_func")))
    {
        for (const auto& section : extra_sections.at("vertex_func"))
        {
            shader += section;
        }
    }

    shader += vertexShaderMainHeader;

    if (find(extra_sections, string("vertex_main")))
    {
        for (const auto& section : extra_sections.at("vertex_main"))
        {
            shader += section;
        }
    }

    shader += vertexShaderMainBody;

    if (normal_map)
    {
        shader += vertexShaderNormalMapMain;
    }

    if (dir_light)
    {
        shader += vertexShaderDirLightMain;
    }

    if (point_lights)
    {
        shader += vertexShaderPointLightMain;
    }

    shader += vertexShaderMainFooter;

    return shader;
}

string fragmentShaderHeader = R"(
#version 430
)";

string fragmentShaderPointLightHeader = R"(
#define MAX_POINT_LIGHTS 4
)";

string fragmentShaderIn = R"(

in vec3 pos_in_world;
in vec3 normal_in_camera;
in vec3 eye_dir_in_camera;
in vec2 uv;
)";

string fragmentShaderNormalMapIn = R"(
in vec3 tangent_in_camera;
in vec3 bitangent_in_camera;
)";

string fragmentShaderDirLightIn = R"(
in vec3 dir_light_dir_in_camera;
)";

string fragmentShaderPointLightIn = R"(
in vec3 point_light_dir_in_camera[MAX_POINT_LIGHTS];
)";

string fragmentShaderOut = R"(

out vec4 color;
)";

string fragmentShaderUniforms = R"(

uniform vec3 diffuse;
uniform vec3 ambient;
uniform vec3 emissive;
uniform float specular = 1.0f;
uniform vec3 ambient_light_strength = vec3(0,0,0);
uniform vec3 specular_color = vec3(0.3,0.3,0.3);
uniform float opacity = 1.0f;
uniform mat4 cam_from_model;
)";

string fragmentShaderTexUniforms = R"(
uniform sampler2D tex_sampler;
)";

string fragmentShaderNormalMapUniforms = R"(
uniform sampler2D normal_sampler;
)";

string fragmentShaderDirLightUniforms = R"(
uniform float dir_light_strength = 0.0f;
)";

string fragmentShaderPointLightUniforms = R"(
uniform vec3 point_light_pos_in_world[MAX_POINT_LIGHTS];
uniform int num_point_lights;
uniform float point_light_strength[MAX_POINT_LIGHTS];
)";

string fragmentShaderMain = R"(

void main()
{
    // Eye vector (towards the camera)
    vec3 E = normalize(eye_dir_in_camera);

    float specular_power = 5.0f;
    vec3 diffuse_light_strength = vec3(0,0,0);
    vec3 specular_light_strength = vec3(0,0,0);
)";

string fragmentShaderTexMain = R"(
    vec4 tex_rgba = texture( tex_sampler, uv ).rgba;
    vec3 tex_rgb = tex_rgba.rgb;
    vec3 ambient_color = tex_rgb * ambient;
    vec3 diffuse_color = tex_rgb * diffuse;
    vec3 emissive_color = tex_rgb * emissive;
)";

string fragmentShaderNoTexMain = R"(
    vec4 tex_rgba = vec4(1,1,1,1);
    vec3 ambient_color = ambient;
    vec3 diffuse_color = diffuse;
    vec3 emissive_color = emissive;
)";

string fragmentShaderNormalMapMain = R"(
    vec3 map_normal = 2*texture( normal_sampler, uv ).xyz - vec3(1,1,1);
    vec3 n = normalize( map_normal.x * tangent_in_camera + map_normal.y * bitangent_in_camera + map_normal.z * normal_in_camera );
)";

string fragmentShaderNoNormalMapMain = R"(
    vec3 n = normalize( normal_in_camera );
)";

string fragmentShaderDirLightMain = R"(
    vec3 dir_light_color = vec3(1,1,1);

    float dirLightCosTheta = clamp(dot(n,dir_light_dir_in_camera), 0, 1);

    vec3 dir_R = reflect(-dir_light_dir_in_camera,n);
    float dir_specular_cos = clamp(dot(E, dir_R), 0, 1);

    vec3 dir_diffuse_light = dir_light_color * dir_light_strength * dirLightCosTheta;
    vec3 dir_specular_light = dir_light_color * dir_light_strength * pow(dir_specular_cos, specular_power) * specular;

    diffuse_light_strength += dir_diffuse_light;
    specular_light_strength += dir_specular_light;
)";

string fragmentShaderPointLightMain = R"(
    vec3 point_light_color = vec3(1,1,1);

    for (int i = 0; i < num_point_lights; i++)
    {
        // Distance to the light
        float distance = length(point_light_pos_in_world[i] - pos_in_world);

        // Direction of the light (from the fragment to the light)
        vec3 l = normalize(point_light_dir_in_camera[i]);
        // Cosine of the angle between the normal and the light direction, 
        // clamped above 0
        //  - light is at the vertical of the triangle -> 1
        //  - light is perpendicular to the triangle -> 0
        //  - light is behind the triangle -> 0
        float pointLightCosTheta = clamp(dot(n,l), 0, 1);

        // Direction in which the triangle reflects the light
        vec3 point_R = reflect(-l,n);
        // Cosine of the angle between the Eye vector and the Reflect vector,
        // clamped to 0
        //  - Looking into the reflection -> 1
        //  - Looking elsewhere -> < 1
        float point_specular_cos = clamp(dot(E, point_R), 0, 1);

        vec3 point_diffuse_light = point_light_color * point_light_strength[i] * pointLightCosTheta / (distance*distance);
        vec3 point_specular_light = point_light_color * point_light_strength[i] * pow(point_specular_cos, specular_power) / (distance*distance) * specular;

        diffuse_light_strength += point_diffuse_light;
        specular_light_strength += point_specular_light;
    }
)";

string fragmentShaderMainFooter = R"(
    color.rgb = 
        // Emissive: simulates produced light
        emissive_color +
        // Ambient : simulates indirect lighting
        ambient_color * ambient_light_strength +
        // Diffuse : "color" of the object
        diffuse_color * diffuse_light_strength +
        // Specular : reflective highlight, like a mirror
        specular_color * specular_light_strength;
        
    color.a = tex_rgba.a * opacity;
    
    if (color.a < 0.01f)
        discard;
}
)";



std::string MaterialBuilder::produceFragmentShader() const
{
    string shader = fragmentShaderHeader;

    if (point_lights)
        shader += fragmentShaderPointLightHeader;

    shader += fragmentShaderIn;

    if (normal_map)
    {
        shader += fragmentShaderNormalMapIn;
    }

    if (dir_light)
    {
        shader += fragmentShaderDirLightIn;
    }

    if (point_lights)
    {
        shader += fragmentShaderPointLightIn;
    }

    shader += fragmentShaderOut;

    shader += fragmentShaderUniforms;

    if (tex)
    {
        shader += fragmentShaderTexUniforms;
    }

    if (normal_map)
    {
        shader += fragmentShaderNormalMapUniforms;
    }

    if (dir_light)
    {
        shader += fragmentShaderDirLightUniforms;
    }

    if (point_lights)
    {
        shader += fragmentShaderPointLightUniforms;
    }

    shader += fragmentShaderMain;

    if (tex)
        shader += fragmentShaderTexMain;
    else
        shader += fragmentShaderNoTexMain;

    if (normal_map)
        shader += fragmentShaderNormalMapMain;
    else
        shader += fragmentShaderNoNormalMapMain;

    if (dir_light)
    {
        shader += fragmentShaderDirLightMain;
    }

    if (point_lights)
    {
        shader += fragmentShaderPointLightMain;
    }

    shader += fragmentShaderMainFooter;

    return shader;
}

const std::string BASE64_ALPHABET =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

// FNV-1a 32-bit hash function
uint32_t fnv1a_hash(const std::string& text) {
    uint32_t hash = 2166136261u;
    for (char c : text) {
        hash ^= static_cast<uint8_t>(c);
        hash *= 16777619u;
    }
    return hash;
}

// Convert a 32-bit hash to a base64-url-safe string
std::string hash_to_base64(uint32_t hash, size_t length = 5) {
    std::string out;
    for (size_t i = 0; i < length; ++i) {
        out += BASE64_ALPHABET[hash & 0x3F]; // 6 bits
        hash >>= 6;
    }
    return out;
}

std::string short_hash(const std::string& text, size_t length = 5) {
    return hash_to_base64(fnv1a_hash(text), length);
}

std::string MaterialBuilder::getTag() const
{
    std::string tag = "mat";
    if (tex)
        tag += "_tex";
    if (normal_map)
        tag += "_nm";
    if (dir_light)
        tag += "_dl";
    if (point_lights)
        tag += "_pl";
    for (const auto& sections : extra_sections)
    {
        for (const auto& section : sections.second)
        {
            tag += "_" + short_hash(sections.first + section);
        }
    }
    return tag;
}

std::shared_ptr<Material> MaterialBuilder::build() const
{
    auto tag = getTag();

    auto material = std::make_shared<Material>();

    material->tag = tag;
    material->program_id = findProgram(tag);
    if (material->program_id == std::numeric_limits<GLuint>::max())
        material->program_id = buildProgram(tag, produceVertexShader(), produceFragmentShader());

    material->pos_attr_loc = glGetAttribLocation(material->program_id, "pos_in_model");
    material->uv_attr_loc = glGetAttribLocation(material->program_id, "in_uv");
    material->norm_attr_loc = glGetAttribLocation(material->program_id, "normal_in_model");

    material->mvp_matrix_id = glGetUniformLocation(material->program_id, "proj_from_model");
    material->view_matrix_id = glGetUniformLocation(material->program_id, "cam_from_world");
    material->model_matrix_id = glGetUniformLocation(material->program_id, "world_from_model");
    material->modelview_matrix_id = glGetUniformLocation(material->program_id, "cam_from_model");
    material->diffuse_id = glGetUniformLocation(material->program_id, "diffuse");
    material->ambient_id = glGetUniformLocation(material->program_id, "ambient");
    material->emissive_id = glGetUniformLocation(material->program_id, "emissive");
    material->specular_id = glGetUniformLocation(material->program_id, "specular");
    material->light_ambient_id = glGetUniformLocation(material->program_id, "ambient_light_strength");

    if (dir_light)
    {
        material->light_dir_id = glGetUniformLocation(material->program_id, "dir_light_dir_in_world");
        material->light_dir_strength_id = glGetUniformLocation(material->program_id, "dir_light_strength");
    }

    if (point_lights)
    {
        material->num_point_lights_id = glGetUniformLocation(material->program_id, "num_point_lights");

        for (size_t i = 0; i < Material::MAX_NUM_POINT_LIGHTS; i++)
        {
            material->light_point_pos_id[i] = glGetUniformLocation(material->program_id, ("point_light_pos_in_world["+std::to_string(i)+"]").c_str());
            material->light_point_strength_id[i] = glGetUniformLocation(material->program_id, ("point_light_strength["+std::to_string(i)+"]").c_str());
        }
    }

    material->opacity_id = glGetUniformLocation(material->program_id, "opacity");

    if (tex)
        material->tex_sampler_id  = glGetUniformLocation(material->program_id, "tex_sampler");

    if (normal_map)
    {
        material->tangent_attr_loc = glGetAttribLocation(material->program_id, "tangent_in_model");
        material->bitangent_attr_loc = glGetAttribLocation(material->program_id, "bitangent_in_model");

        material->normal_sampler_id  = glGetUniformLocation(material->program_id, "normal_sampler");
    }

    if (time)
        material->time_id = glGetUniformLocation(material->program_id, "time");

    return material;
}

}