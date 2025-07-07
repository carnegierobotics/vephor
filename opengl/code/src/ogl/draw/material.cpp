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

void MaterialProgram::activate(Window* window, const TransformSim3& world_from_body, const MaterialState& state)
{
    glUseProgram(program_id);

    Mat4 M = world_from_body.matrix();
    Mat4 V = window->getCamFromWorldMatrix();
    if (infinite_depth)
    {
        V.block(0,3,3,1).setZero();
    }
	Mat4 P = window->getProjectionMatrix();
    Mat4 VM = V * M;
    Mat4 PVM = P * VM;

    glUniformMatrix4fv(proj_from_model_matrix_id, 1, GL_FALSE, PVM.data());

    if (proj_from_camera_matrix_id != std::numeric_limits<GLuint>::max())
    {
        glUniformMatrix4fv(proj_from_camera_matrix_id, 1, GL_FALSE, P.data());
    }

    if (world_from_model_matrix_id != std::numeric_limits<GLuint>::max())
    {
        glUniformMatrix4fv(world_from_model_matrix_id, 1, GL_FALSE, M.data());
    }

    if (camera_from_world_matrix_id != std::numeric_limits<GLuint>::max())
    {
        glUniformMatrix4fv(camera_from_world_matrix_id, 1, GL_FALSE, V.data());
    }

    if (camera_from_model_matrix_id != std::numeric_limits<GLuint>::max())
    {
        glUniformMatrix4fv(camera_from_model_matrix_id, 1, GL_FALSE, VM.data());
    }

    if (diffuse_id != std::numeric_limits<GLuint>::max())
    {
        glUniform3fv(diffuse_id, 1, state.diffuse.data());
        glUniform3fv(ambient_id, 1, state.ambient.data());
        glUniform3fv(emissive_id, 1, state.emissive.data());
        glUniform1f(specular_id, state.specular);
        glUniform1f(opacity_id, state.opacity);

        Vec3 ambient_light_strength = window->getAmbientLight();
        glUniform3fv(light_ambient_id, 1, ambient_light_strength.data());
    }

    if (tex_sampler_id != std::numeric_limits<GLuint>::max())
    {
        auto tex = state.tex;
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
        if (!state.normal_map)
	    {
            throw std::runtime_error("No normal map found in material.");
        }

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, state.normal_map->getID());
        glUniform1i(normal_sampler_id, 1);
    }

    if (cube_tex_sampler_id != std::numeric_limits<GLuint>::max())
    {
        if (!state.cube_tex)
        {
            throw std::runtime_error("No cube texture found in material.");
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, state.cube_tex->getID());
        glUniform1i(cube_tex_sampler_id, 0);
    }

    if (dir_light_dir_id != std::numeric_limits<GLuint>::max())
    {
        auto dir_light = window->getDirLight();

        glUniform3fv(dir_light_dir_id, 1, dir_light.pos.data());
        glUniform1f(dir_light_strength_id, dir_light.strength);
    }

    Mat4 dir_light_proj_from_world;
    shared_ptr<Texture> dir_light_shadow_map;
    if (dir_light_proj_from_world_id != std::numeric_limits<GLuint>::max() && 
        window->getDirLightShadowInfo(
            dir_light_proj_from_world,
            dir_light_shadow_map
        ))
    {
        glUniformMatrix4fv(dir_light_proj_from_world_id, 1, GL_FALSE, dir_light_proj_from_world.data());

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, dir_light_shadow_map->getID());
        glUniform1i(dir_light_shadow_map_sampler_id, 2);
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

    if (size_id != std::numeric_limits<GLuint>::max())
    {
        glUniform1f(size_id, state.size);
    }

    if (aspect_id != std::numeric_limits<GLuint>::max())
    {
        glUniform1f(aspect_id, (float)window->getSize()[0] / (float)window->getSize()[1]);
    }

    if (screen_size_id != std::numeric_limits<GLuint>::max())
    {
        Vec2 size = window->getSize().cast<float>();
        glUniform2fv(screen_size_id, 1, size.data());
    }
}

void MaterialProgram::deactivate()
{
}

string vertexShaderHeader = R"(
#version 430
)";

string vertexShaderPointLightHeader = R"(
#define MAX_POINT_LIGHTS 4
)";

std::vector<std::string> vertexShaderLayout = {
    "vec3 in_pos_in_model"
};

std::vector<std::string> vertexShaderTexLayout = {
    "vec2 in_uv"
};

std::vector<std::string> vertexShaderNormalMapLayout = {
    "vec3 in_tangent_in_model",
    "vec3 in_bitangent_in_model"
};

std::vector<std::string> vertexShaderLightingLayout = {
    "vec3 in_normal_in_model"
};

std::vector<std::string> vertexShaderVertexColorLayout = {
    "vec4 in_color"
};

std::vector<std::string> vertexShaderOffsetLayout = {
    "vec3 in_offset"
};

std::vector<std::string> vertexShaderInstancedSizeLayout = {
    "float in_size"
};


string vertexShaderOut = R"(

)";

string vertexShaderLightingOut = R"(
out vec3 vo_normal_in_camera;
out vec3 vo_eye_dir_in_camera;
)";

string vertexShaderTexOut = R"(
out vec2 vo_uv;
)";

string vertexShaderNormalMapOut = R"(
out vec3 vo_tangent_in_camera;
out vec3 vo_bitangent_in_camera;
)";

string vertexShaderCubeTexOut = R"(
out vec3 vo_cube_vec;
)";

string vertexShaderDirLightOut = R"(
out vec3 vo_dir_light_dir_in_camera;
)";

string vertexShaderPointLightOut = R"(
out vec3 vo_pos_in_world;
out vec3 vo_point_light_dir_in_camera[MAX_POINT_LIGHTS];
)";

string vertexShaderVertexColorOut = R"(
out vec4 vo_color;
)";

string vertexShaderUniforms = R"(

uniform mat4 proj_from_model;
)";

string vertexShaderBillboardUniforms = R"(
uniform mat4 proj_from_camera;
)";

string vertexShaderLightingUniforms = R"(
uniform mat4 cam_from_world;
uniform mat4 world_from_model;
)";

string vertexShaderCamFromModelUniforms = R"(
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

string vertexShaderUniformSizeUniforms = R"(
uniform float size;
)";

string vertexShaderScreenSpaceUniforms = R"(
uniform float aspect;
)";

string vertexShaderWorldFromModelUniforms = R"(
uniform mat4 world_from_model;
)";

string vertexShaderMainHeader = R"(

void main()
{
    vec3 curr_pos_in_model = in_pos_in_model;
)";

string vertexShaderLightingMainHeader = R"(
    vec3 curr_normal_in_model = in_normal_in_model;
)";

string vertexShaderUniformSizeMainHeader = R"(
    curr_pos_in_model *= size;
)";

string vertexShaderInstancedSizeMainHeader = R"(
    curr_pos_in_model *= in_size;
)";

string vertexShaderMain = R"(
    gl_Position = proj_from_model * vec4(curr_pos_in_model, 1);
)";

string vertexShaderOffsetMain = R"(
    vec4 center_in_proj = proj_from_model * vec4(in_offset, 1);
    gl_Position = proj_from_model * vec4(curr_pos_in_model, 1) + center_in_proj;
)";

string vertexShaderBillboardMain = R"(
    gl_Position = proj_from_camera * vec4(curr_pos_in_model, 1);
)";

string vertexShaderBillboardOffsetMain = R"(
    vec3 pos_in_camera = curr_pos_in_model + (cam_from_model * vec4(in_offset,1)).xyz;
    gl_Position = proj_from_camera * vec4(pos_in_camera, 1);
)";

string vertexShaderScreenSpaceMain = R"(
    vec4 center_in_proj = proj_from_model * vec4(in_offset, 1);
    vec4 ss_offset = vec4(2 * curr_pos_in_model.x / aspect, 2 * curr_pos_in_model.y, 0.0, 0.0) * center_in_proj.w; 
    gl_Position = ss_offset + center_in_proj;
)";

string vertexShaderInfiniteDepthMain = R"(
    vec4 id_pos = proj_from_model * vec4(curr_pos_in_model, 1);
    gl_Position = id_pos.xyww;
)";

string vertexShaderLightingMain = R"(
    vec3 pos_in_world = (world_from_model * vec4(curr_pos_in_model,1)).xyz;
    
    // Vector that goes from the vertex to the camera, in camera space.
    // In camera space, the camera is at the origin (0,0,0).
    vec3 pos_in_camera = (cam_from_world * vec4(pos_in_world,1)).xyz;
    vo_eye_dir_in_camera = vec3(0,0,0) - pos_in_camera;

    // Normal of the the vertex, in camera space
    // Only correct if cam_from_model does not scale the model ! Use its inverse transpose if not.
    vo_normal_in_camera = (cam_from_model * vec4(curr_normal_in_model,0)).xyz; 
)";

string vertexShaderTexMain = R"(
    vo_uv = in_uv;
)";

string vertexShaderCubeTexMain = R"(
    vo_cube_vec = curr_pos_in_model;
)";

string vertexShaderNormalMapMain = R"(
    vo_tangent_in_camera = (cam_from_model * vec4(in_tangent_in_model,0)).xyz; 
    vo_bitangent_in_camera = (cam_from_model * vec4(in_bitangent_in_model,0)).xyz; 
)";

string vertexShaderDirLightMain = R"(
    vo_dir_light_dir_in_camera = (cam_from_world * vec4(dir_light_dir_in_world,0)).xyz;
)";

string vertexShaderPointLightMain = R"(
    // Position of the vertex, in worldspace : M * position
    vo_pos_in_world = pos_in_world;

    for (int i = 0; i < num_point_lights; i++)
    {
        // Vector that goes from the vertex to the light, in camera space.
        vec3 point_light_pos_in_camera = (cam_from_world * vec4(point_light_pos_in_world[i],1)).xyz;
        vo_point_light_dir_in_camera[i] = point_light_pos_in_camera + vo_eye_dir_in_camera;
    }
)";

string vertexShaderVertexColorMain = R"(
    vo_color = in_color;
)";

string vertexShaderMainFooter = R"(}
)";

std::string MaterialBuilder::produceVertexShader() const
{
    bool lighting = materials && (dir_light || point_lights);

    string shader = vertexShaderHeader;

    if (point_lights)
        shader += vertexShaderPointLightHeader;

    shader += "\n";

    std::vector<std::string> layout = vertexShaderLayout;

    if (tex || normal_map)
        extend(layout, vertexShaderTexLayout);

    if (lighting)
        extend(layout, vertexShaderLightingLayout);

    if (normal_map)
        extend(layout, vertexShaderNormalMapLayout);

    if (vertex_color)
        extend(layout, vertexShaderVertexColorLayout);

    if (offset)
        extend(layout, vertexShaderOffsetLayout);

    if (instanced_size)
        extend(layout, vertexShaderInstancedSizeLayout);

    for (int i = 0; i < layout.size(); i++)
    {
        shader += "layout(location = "+std::to_string(i)+") in " + layout[i] + ";\n";
    }

    shader += vertexShaderOut;

    if (lighting)
        shader += vertexShaderLightingOut;

    if (tex || normal_map)
        shader += vertexShaderTexOut;

    if (normal_map)
        shader += vertexShaderNormalMapOut;

    if (cube_tex)
        shader += vertexShaderCubeTexOut;

    if (dir_light)
        shader += vertexShaderDirLightOut;

    if (point_lights)
        shader += vertexShaderPointLightOut;

    if (vertex_color)
        shader += vertexShaderVertexColorOut;

    shader += vertexShaderUniforms;

    if (billboard)
        shader += vertexShaderBillboardUniforms;

    if (lighting)
        shader += vertexShaderLightingUniforms;

    if (billboard || lighting)
        shader += vertexShaderCamFromModelUniforms;

    if (dir_light)
        shader += vertexShaderDirLightUniforms;

    if (point_lights)
        shader += vertexShaderPointLightUniforms;

    if (time)
        shader += vertexShaderTimeUniforms;

    if (uniform_size)
        shader += vertexShaderUniformSizeUniforms;

    if (screen_space)
        shader += vertexShaderScreenSpaceUniforms;

    if (vert_world_from_model)
        shader += vertexShaderWorldFromModelUniforms;

    if (find(extra_sections, string("vertex_func")))
    {
        for (const auto& section : extra_sections.at("vertex_func"))
        {
            shader += section;
        }
    }

    shader += vertexShaderMainHeader;

    if (lighting)
        shader += vertexShaderLightingMainHeader;

    if (uniform_size)
        shader += vertexShaderUniformSizeMainHeader;

    if (instanced_size)
        shader += vertexShaderInstancedSizeMainHeader;

    if (find(extra_sections, string("vertex_main")))
    {
        for (const auto& section : extra_sections.at("vertex_main"))
        {
            shader += section;
        }
    }

    if (infinite_depth)
        shader += vertexShaderInfiniteDepthMain;
    else if (screen_space)
        shader += vertexShaderScreenSpaceMain;
    else if (billboard)
    {
        if (offset)
            shader += vertexShaderBillboardOffsetMain;
        else
            shader += vertexShaderBillboardMain;
    }
    else 
    {
        if (offset)
            shader += vertexShaderOffsetMain;
        else
            shader += vertexShaderMain;
    }

    if (tex || normal_map)
        shader += vertexShaderTexMain;

    if (cube_tex)
        shader += vertexShaderCubeTexMain;

    if (normal_map)
        shader += vertexShaderNormalMapMain;

    if (lighting)
        shader += vertexShaderLightingMain;

    if (dir_light)
        shader += vertexShaderDirLightMain;

    if (point_lights)
        shader += vertexShaderPointLightMain;

    if (vertex_color)
        shader += vertexShaderVertexColorMain;

    shader += vertexShaderMainFooter;

    return shader;
}

string fragmentShaderHeader = R"(
#version 430
)";

string fragmentShaderPointLightHeader = R"(
#define MAX_POINT_LIGHTS 4
)";

string fragmentShaderTexIn = R"(
in vec2 vo_uv;
)";

string fragmentShaderCubeTexIn = R"(
in vec3 vo_cube_vec;
)";

string fragmentShaderLightingIn = R"(
in vec3 vo_normal_in_camera;
in vec3 vo_eye_dir_in_camera;
)";

string fragmentShaderNormalMapIn = R"(
in vec3 vo_tangent_in_camera;
in vec3 vo_bitangent_in_camera;
)";

string fragmentShaderDirLightIn = R"(
in vec3 vo_dir_light_dir_in_camera;
)";

string fragmentShaderPointLightIn = R"(
in vec3 vo_pos_in_world;
in vec3 vo_point_light_dir_in_camera[MAX_POINT_LIGHTS];
)";

string fragmentShaderVertexColorIn = R"(
in vec4 vo_color;
)";

string fragmentShaderOut = R"(

out vec4 out_color;
)";

string fragmentShaderMaterialsUniforms = R"(
uniform vec3 diffuse;
uniform vec3 ambient;
uniform vec3 emissive;
uniform float specular = 1.0f;
uniform vec3 specular_color = vec3(0.3,0.3,0.3);
uniform float opacity = 1.0f;
uniform vec3 ambient_light_strength = vec3(0,0,0);
)";

string fragmentShaderTexUniforms = R"(
uniform sampler2D tex_sampler;
)";

string fragmentShaderNormalMapUniforms = R"(
uniform sampler2D normal_sampler;
)";

string fragmentShaderCubeTexUniforms = R"(
uniform samplerCube cube_tex_sampler;
)";

string fragmentShaderDirLightUniforms = R"(
uniform float dir_light_strength = 0.0f;
uniform vec3 dir_light_color = vec3(1,1,1);
)";

string fragmentShaderDirLightShadowsUniforms = R"(
uniform mat4 dir_light_proj_from_world;
uniform sampler2D dir_light_shadow_map_sampler;
)";

string fragmentShaderPointLightUniforms = R"(
uniform vec3 point_light_pos_in_world[MAX_POINT_LIGHTS];
uniform int num_point_lights;
uniform float point_light_strength[MAX_POINT_LIGHTS];
)";

string fragmentShaderScreenSpaceTexCoordsUniforms = R"(
uniform vec2 screen_size;
)";

string fragmentShaderTimeUniforms = R"(
uniform float time;
)";

string fragmentShaderCamFromWorldUniforms = R"(
uniform mat4 cam_from_world;
)";

string fragmentShaderMain = R"(

void main()
{)";

string fragmentShaderScreenSpaceTexCoordsMain = R"(
    vec2 uv = gl_FragCoord.xy / screen_size;
    vec4 tex_rgba = texture(tex_sampler, uv).rgba;
)";

string fragmentShaderTexMain = R"(
    vec4 tex_rgba = texture(tex_sampler, vo_uv).rgba;
)";

string fragmentShaderCubeTexMain = R"(
    vec4 tex_rgba = texture(cube_tex_sampler, vo_cube_vec);
)";

string fragmentShaderNoTexMain = R"(
    vec4 tex_rgba = vec4(1,1,1,1);
)";

string fragmentShaderNormalMapMain = R"(
    vec3 map_normal = 2*texture(normal_sampler, vo_uv).xyz - vec3(1,1,1);
    vec3 n = normalize( 
        map_normal.x * vo_tangent_in_camera + 
        map_normal.y * vo_bitangent_in_camera + 
        map_normal.z * vo_normal_in_camera );
)";

string fragmentShaderNoNormalMapMain = R"(
    vec3 n = normalize( vo_normal_in_camera );
)";

string fragmentShaderMaterialsMain = R"(
    vec3 diffuse_light_strength = vec3(0,0,0);
    vec3 specular_light_strength = vec3(0,0,0);
)";

string fragmentShaderLightingMain = R"(
    // Eye vector (towards the camera)
    vec3 E = normalize(vo_eye_dir_in_camera);

    float specular_power = 5.0f;
)";

string fragmentShaderDirLightMain = R"(
    float dirLightCosTheta = clamp(dot(n,vo_dir_light_dir_in_camera), 0, 1);

    vec3 dir_R = reflect(-vo_dir_light_dir_in_camera,n);
    float dir_specular_cos = clamp(dot(E, dir_R), 0, 1);

    vec3 dir_diffuse_light = dir_light_color * dir_light_strength * dirLightCosTheta;
    vec3 dir_specular_light = dir_light_color * dir_light_strength * pow(dir_specular_cos, specular_power) * specular;

    diffuse_light_strength += dir_diffuse_light;
    specular_light_strength += dir_specular_light;
)";

string fragmentShaderDirLightShadowsMain = R"(
    vec4 pos_in_dir_light = dir_light_proj_from_world * vec4(vo_pos_in_world, 1.0);
    vec3 proj_pos_in_dir_light = pos_in_dir_light.xyz / pos_in_dir_light.w * 0.5 + 0.5;
    float depth = texture(dir_light_shadow_map_sampler, proj_pos_in_dir_light.xy).r;
    
    if (clamp(proj_pos_in_dir_light.z, 0, 1) < depth + 0.001)
    {
        float dirLightCosTheta = clamp(dot(n,vo_dir_light_dir_in_camera), 0, 1);

        vec3 dir_R = reflect(-vo_dir_light_dir_in_camera,n);
        float dir_specular_cos = clamp(dot(E, dir_R), 0, 1);

        vec3 dir_diffuse_light = dir_light_color * dir_light_strength * dirLightCosTheta;
        vec3 dir_specular_light = dir_light_color * dir_light_strength * pow(dir_specular_cos, specular_power) * specular;

        diffuse_light_strength += dir_diffuse_light;
        specular_light_strength += dir_specular_light;
    }
)";

string fragmentShaderPointLightMain = R"(
    vec3 point_light_color = vec3(1,1,1);

    for (int i = 0; i < num_point_lights; i++)
    {
        // Distance to the light
        float distance = length(point_light_pos_in_world[i] - vo_pos_in_world);

        // Direction of the light (from the fragment to the light)
        vec3 l = normalize(vo_point_light_dir_in_camera[i]);
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

string fragmentShaderMaterialsMainEnd = R"(
    out_color.rgb = 
        // Emissive: simulates produced light
        emissive +
        // Ambient : simulates indirect lighting
        ambient * ambient_light_strength +
        // Diffuse : "color" of the object
        diffuse * diffuse_light_strength;

    out_color.rgb *= tex_rgba.rgb;

    // Specular : reflective highlight, like a mirror
    out_color.rgb += specular_color * specular_light_strength;
        
    out_color.a = tex_rgba.a * opacity;
)";

string fragmentShaderNoMaterialsMainEnd = R"(
    out_color = tex_rgba;
)";

string fragmentShaderVertexColorMainEnd = R"(
    out_color *= vo_color;
)";

string fragmentShaderMainFooter = R"(
    if (out_color.a < 0.01f)
        discard;
}
)";

string fragmentShaderNoColorMainFooter = R"(
}
)";




std::string MaterialBuilder::produceFragmentShader() const
{
    bool lighting = materials && (dir_light || point_lights);

    string shader = fragmentShaderHeader;

    if (point_lights)
        shader += fragmentShaderPointLightHeader;

    shader += "\n";

    if (tex || normal_map)
        shader += fragmentShaderTexIn;

    if (cube_tex)
        shader += fragmentShaderCubeTexIn;

    if (normal_map)
        shader += fragmentShaderNormalMapIn;

    if (lighting)
        shader += fragmentShaderLightingIn;

    if (dir_light)
        shader += fragmentShaderDirLightIn;

    if (point_lights)
        shader += fragmentShaderPointLightIn;

    if (vertex_color)
        shader += fragmentShaderVertexColorIn;

    if (!no_color)
        shader += fragmentShaderOut;

    shader += "\n";

    if (materials)
        shader += fragmentShaderMaterialsUniforms;

    if (tex)
        shader += fragmentShaderTexUniforms;

    if (normal_map)
        shader += fragmentShaderNormalMapUniforms;

    if (cube_tex)
        shader += fragmentShaderCubeTexUniforms;

    if (dir_light)
        shader += fragmentShaderDirLightUniforms;

    if (dir_light_shadows)
        shader += fragmentShaderDirLightShadowsUniforms;

    if (point_lights)
        shader += fragmentShaderPointLightUniforms;

    if (screen_space_tex_coords)
        shader += fragmentShaderScreenSpaceTexCoordsUniforms;

    if (time)
        shader += fragmentShaderTimeUniforms;

    if (frag_cam_from_world)
        shader += fragmentShaderCamFromWorldUniforms;

    if (find(extra_sections, string("frag_func")))
    {
        for (const auto& section : extra_sections.at("frag_func"))
        {
            shader += section;
        }
    }

    shader += fragmentShaderMain;

    if (lighting)
        shader += fragmentShaderLightingMain;

    if (lighting)
    {
        if (normal_map)
            shader += fragmentShaderNormalMapMain;
        else
            shader += fragmentShaderNoNormalMapMain;
    }

    if (!no_color)
    {
        if (find(extra_sections, string("frag_tex")))
        {
            for (const auto& section : extra_sections.at("frag_tex"))
            {
                shader += section;
            }
        }
        else if (screen_space_tex_coords)
            shader += fragmentShaderScreenSpaceTexCoordsMain;
        else if (cube_tex)
            shader += fragmentShaderCubeTexMain;
        else if (tex)
            shader += fragmentShaderTexMain;
        else
            shader += fragmentShaderNoTexMain;
    }

    if (materials)
        shader += fragmentShaderMaterialsMain;

    if (dir_light_shadows)
        shader += fragmentShaderDirLightShadowsMain;
    else if (dir_light)
        shader += fragmentShaderDirLightMain;

    if (point_lights)
        shader += fragmentShaderPointLightMain;

    if (find(extra_sections, string("frag_main")))
    {
        for (const auto& section : extra_sections.at("frag_main"))
        {
            shader += section;
        }
    }

    if (!no_color)
    {
        if (materials)
            shader += fragmentShaderMaterialsMainEnd;
        else
            shader += fragmentShaderNoMaterialsMainEnd;
    }

    if (vertex_color)
        shader += fragmentShaderVertexColorMainEnd;

    if (no_color)
        shader += fragmentShaderNoColorMainFooter;
    else
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
    if (cube_tex)
        tag += "_ctex";
    if (dir_light)
        tag += "_dl";
    if (dir_light_shadows)
        tag += "_dlsh";
    if (point_lights)
        tag += "_pl";
    if (time)
        tag += "_t";
    if (vertex_color)
        tag += "_vc";
    if (!materials)
        tag += "_nomat";
    if (offset)
        tag += "_o";
    if (uniform_size)
        tag += "_us";
    if (instanced_size)
        tag += "_is";
    if (billboard)
        tag += "_b";
    if (screen_space)
        tag += "_ss";
    if (infinite_depth)
        tag += "_id";
    if (screen_space_tex_coords)
        tag += "_sstc";
    if (no_color)
        tag += "_nocol";
    if (vert_world_from_model)
        tag += "_vwfm";
    if (frag_cam_from_world)
        tag += "_fcfw";
    for (const auto& sections : extra_sections)
    {
        for (const auto& section : sections.second)
        {
            tag += "_" + short_hash(sections.first + section);
        }
    }
    return tag;
}

void MaterialBuilder::saveShaders() const
{
    auto tag = getTag();
    auto vert = produceVertexShader();
    auto frag = produceFragmentShader();

    std::ofstream v_out("/tmp/vert_"+tag);
    v_out << vert << std::endl;
    v_out.close();

    std::ofstream f_out("/tmp/frag_"+tag);
    f_out << frag << std::endl;
    f_out.close();
}

unordered_map<string, shared_ptr<MaterialProgram>> material_bank;

std::shared_ptr<MaterialProgram> MaterialBuilder::build() const
{
    bool lighting = materials && (dir_light || point_lights);

    if (!lighting && (dir_light || point_lights))
        throw std::runtime_error("Must have lighting enabled to use directional or point lights.");

    if (!lighting && normal_map)
        throw std::runtime_error("Must have lighting enabled to use normal maps.");

    if (uniform_size && instanced_size)
        throw std::runtime_error("Can't use both uniform size and instanced size.");

    if (screen_space && (!billboard || !offset))
        throw std::runtime_error("Screen space must be used with billboard and offset.");

    if (screen_space_tex_coords && !tex)
        throw std::runtime_error("Can't use screen space tex coords without a texture.");

    if (dir_light_shadows && !dir_light)
        throw std::runtime_error("Can't use dir light shadows without dir lights.");

    auto tag = getTag();

    if (find(material_bank, tag))
        return material_bank[tag];

    auto material = std::make_shared<MaterialProgram>();

    material->tag = tag;
    material->program_id = findProgram(tag);
    if (material->program_id == std::numeric_limits<GLuint>::max())
        material->program_id = buildProgram(tag, produceVertexShader(), produceFragmentShader());

    material->pos_attr_loc = glGetAttribLocation(material->program_id, "in_pos_in_model");
    material->proj_from_model_matrix_id = glGetUniformLocation(material->program_id, "proj_from_model");
    

    if (tex || normal_map)
    {
        material->uv_attr_loc = glGetAttribLocation(material->program_id, "in_uv");
    }

    if (materials)
    {
        material->diffuse_id = glGetUniformLocation(material->program_id, "diffuse");
        material->ambient_id = glGetUniformLocation(material->program_id, "ambient");
        material->emissive_id = glGetUniformLocation(material->program_id, "emissive");
        material->specular_id = glGetUniformLocation(material->program_id, "specular");
        material->light_ambient_id = glGetUniformLocation(material->program_id, "ambient_light_strength");
    }

    if (lighting)
    {
        material->norm_attr_loc = glGetAttribLocation(material->program_id, "in_normal_in_model");

        material->camera_from_world_matrix_id = glGetUniformLocation(material->program_id, "cam_from_world");
        material->world_from_model_matrix_id = glGetUniformLocation(material->program_id, "world_from_model");
    }

    if (billboard || lighting)
    {
        material->camera_from_model_matrix_id = glGetUniformLocation(material->program_id, "cam_from_model");
    }

    if (offset)
    {
        material->offset_attr_loc = glGetAttribLocation(material->program_id, "in_offset");
    }

    if (vertex_color)
    {
        material->color_attr_loc = glGetAttribLocation(material->program_id, "in_color");
    }

    if (instanced_size)
    {
        material->size_attr_loc = glGetAttribLocation(material->program_id, "in_size");
    }

    if (uniform_size)
    {
        material->size_id = glGetUniformLocation(material->program_id, "size");
    }

    if (screen_space)
    {
        material->aspect_id = glGetUniformLocation(material->program_id, "aspect");
    }

    if (screen_space_tex_coords)
    {
        material->screen_size_id = glGetUniformLocation(material->program_id, "screen_size");
    }

    if (billboard)
    {
        material->proj_from_camera_matrix_id = glGetUniformLocation(material->program_id, "proj_from_camera");
    }

    if (dir_light)
    {
        material->dir_light_dir_id = glGetUniformLocation(material->program_id, "dir_light_dir_in_world");
        material->dir_light_strength_id = glGetUniformLocation(material->program_id, "dir_light_strength");
    }

    if (dir_light_shadows)
    {
        material->dir_light_proj_from_world_id = glGetUniformLocation(material->program_id, "dir_light_proj_from_world");
        material->dir_light_shadow_map_sampler_id = glGetUniformLocation(material->program_id, "dir_light_shadow_map_sampler");
    }

    if (point_lights)
    {
        material->num_point_lights_id = glGetUniformLocation(material->program_id, "num_point_lights");

        for (size_t i = 0; i < MaterialProgram::MAX_NUM_POINT_LIGHTS; i++)
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
        material->tangent_attr_loc = glGetAttribLocation(material->program_id, "in_tangent_in_model");
        material->bitangent_attr_loc = glGetAttribLocation(material->program_id, "in_bitangent_in_model");

        material->normal_sampler_id  = glGetUniformLocation(material->program_id, "normal_sampler");
    }

    if (cube_tex)
        material->cube_tex_sampler_id  = glGetUniformLocation(material->program_id, "cube_tex_sampler");

    if (time)
        material->time_id = glGetUniformLocation(material->program_id, "time");

    material->infinite_depth = infinite_depth;

    material_bank[tag] = material;

    return material;
}

MaterialProgramSet MaterialBuilder::buildSet(bool simple_depth) const
{
    MaterialProgramSet set;

    set["main"] = build();

    MaterialBuilder depth_builder = *this;
    if (simple_depth)
    {
        depth_builder.tex = false;
        depth_builder.normal_map = false;
        depth_builder.cube_tex = false;
        depth_builder.dir_light = false;
        depth_builder.point_lights = false;
        depth_builder.vertex_color = false;
        depth_builder.materials = true;
        depth_builder.screen_space_tex_coords = false;
        depth_builder.no_color = true;
        depth_builder.dir_light_shadows = false;
    }
    set["depth"] = depth_builder.build();

    MaterialBuilder dir_light_shadow_builder = *this;
    if (dir_light_shadow_builder.dir_light)
        dir_light_shadow_builder.dir_light_shadows = true;
    set["shadows"] = dir_light_shadow_builder.build();

    return set;
}

}