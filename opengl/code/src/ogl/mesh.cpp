/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Carnegie Robotics, LLC nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CARNEGIE ROBOTICS, LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

#include "vephor/ogl/mesh.h"

namespace vephor
{

struct MeshShader
{
    string vertex;
    string fragment;
};

inline MeshShader produceMeshShader(bool with_normal_map)
{
    string vertexShader = R"(
        #version 430
        layout(location = 0) in vec3 pos_in_model;
        layout(location = 1) in vec2 in_uv;
        layout(location = 2) in vec3 normal_in_model;
    )";

    if (with_normal_map)
    {
        vertexShader += R"(
            layout(location = 3) in vec3 tangent_in_model;
            layout(location = 4) in vec3 bitangent_in_model;
        )";
    }

    vertexShader += R"(
        out vec3 pos_in_world;
        out vec3 normal_in_camera;
    )";

    if (with_normal_map)
    {
        vertexShader += R"(
            out vec3 tangent_in_camera;
            out vec3 bitangent_in_camera;
        )";
    }

    vertexShader += R"(
        out vec3 eye_dir_in_camera;
        out vec3 dir_light_dir_in_camera;
        out vec3 point_light_dir_in_camera;
        out vec2 uv;
        
        uniform mat4 proj_from_model;
        uniform mat4 cam_from_world;
        uniform mat4 world_from_model;
        uniform mat4 cam_from_model;
        uniform vec3 point_light_pos_in_world;
        uniform vec3 dir_light_dir_in_world;
        
        void main()
        {
            gl_Position = proj_from_model * vec4(pos_in_model, 1);
            
            // Position of the vertex, in worldspace : M * position
            pos_in_world = (world_from_model * vec4(pos_in_model,1)).xyz;
            
            // Vector that goes from the vertex to the camera, in camera space.
            // In camera space, the camera is at the origin (0,0,0).
            vec3 pos_in_camera = ( cam_from_world * vec4(pos_in_world,1)).xyz;
            eye_dir_in_camera = vec3(0,0,0) - pos_in_camera;

            // Vector that goes from the vertex to the light, in camera space.
            vec3 point_light_pos_in_camera = ( cam_from_world * vec4(point_light_pos_in_world,1)).xyz;
            point_light_dir_in_camera = point_light_pos_in_camera + eye_dir_in_camera;

            dir_light_dir_in_camera = ( cam_from_world * vec4(dir_light_dir_in_world,0) ).xyz;
            
            // Normal of the the vertex, in camera space
            // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
            normal_in_camera = (cam_from_model * vec4(normal_in_model,0)).xyz; 
    )";

    if (with_normal_map)
    {
        vertexShader += R"(
            tangent_in_camera = (cam_from_model * vec4(tangent_in_model,0)).xyz; 
            bitangent_in_camera = (cam_from_model * vec4(bitangent_in_model,0)).xyz; 
        )";
    }

    vertexShader += R"(
            uv = in_uv;
        }
    )";

    string fragmentShader = R"(
        #version 430

        in vec3 pos_in_world;
        in vec3 normal_in_camera;
    )";

    if (with_normal_map)
    {
        fragmentShader += R"(
            in vec3 tangent_in_camera;
            in vec3 bitangent_in_camera;
        )";
    }

    fragmentShader += R"(
        in vec3 eye_dir_in_camera;
        in vec3 dir_light_dir_in_camera;
        in vec3 point_light_dir_in_camera;
        in vec2 uv;

        out vec4 color;

        uniform vec3 diffuse;
        uniform vec3 ambient;
        uniform vec3 emissive;
        uniform float specular = 1.0f;
        uniform vec3 point_light_pos_in_world;
        uniform sampler2D tex_sampler;
    )";

    if (with_normal_map)
    {
        fragmentShader += R"(
            uniform sampler2D normal_sampler;
        )";
    }

    fragmentShader += R"(
        uniform vec3 ambient_light_strength = vec3(0,0,0);
        uniform float dir_light_strength = 0.0f;
        uniform float point_light_strength = 0.0f;
        uniform float opacity = 1.0f;
        uniform mat4 cam_from_model;

        void main()
        {
            // Light emission properties
            // You probably want to put them as uniforms
            vec3 point_light_color = vec3(1,1,1);
            vec3 dir_light_color = vec3(1,1,1);
            
            // Material properties
            vec4 tex_rgba = texture( tex_sampler, uv ).rgba;
            vec3 tex_rgb = tex_rgba.rgb;
            vec3 MaterialDiffuseColor = tex_rgb * diffuse;
            vec3 MaterialAmbientColor = tex_rgb * ambient;
            vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);
            vec3 MaterialEmissiveColor = tex_rgb * emissive;

            // Distance to the light
            float distance = length( point_light_pos_in_world - pos_in_world );
        )";

        if (with_normal_map)
        {
            fragmentShader += R"(
                vec3 map_normal = 2*texture( normal_sampler, uv ).xyz - vec3(1,1,1);
                vec3 n = normalize( map_normal.x * tangent_in_camera + map_normal.y * bitangent_in_camera + map_normal.z * normal_in_camera );
            )";
        }
        else
        {
            fragmentShader += R"(
                vec3 n = normalize( normal_in_camera );
            )";
        }

        fragmentShader += R"(
            // Direction of the light (from the fragment to the light)
            vec3 l = normalize( point_light_dir_in_camera );
            // Cosine of the angle between the normal and the light direction, 
            // clamped above 0
            //  - light is at the vertical of the triangle -> 1
            //  - light is perpendicular to the triangle -> 0
            //  - light is behind the triangle -> 0
            float pointLightCosTheta = clamp( dot( n,l ), 0,1 );

            float dirLightCosTheta = clamp( dot( n,dir_light_dir_in_camera ), 0,1 );
            
            // Eye vector (towards the camera)
            vec3 E = normalize(eye_dir_in_camera);
            // Direction in which the triangle reflects the light
            vec3 pointR = reflect(-l,n);
            // Cosine of the angle between the Eye vector and the Reflect vector,
            // clamped to 0
            //  - Looking into the reflection -> 1
            //  - Looking elsewhere -> < 1
            float point_specular_cos = clamp( dot( E,pointR ), 0,1 );

            vec3 dir_R = reflect(-dir_light_dir_in_camera,n);
            float dir_specular_cos = clamp( dot( E, dir_R ), 0,1 );
            
            vec3 dir_diffuse_light = dir_light_color * dir_light_strength * dirLightCosTheta;
            vec3 point_diffuse_light = point_light_color * point_light_strength * pointLightCosTheta / (distance*distance);

            vec3 dir_specular_light = dir_light_color * dir_light_strength * pow(dir_specular_cos,5) * specular;
            vec3 point_specular_light = point_light_color * point_light_strength * pow(point_specular_cos,5) / (distance*distance) * specular;

            color.rgb = 
                // Emissive: simulates produced light
                MaterialEmissiveColor +
                // Ambient : simulates indirect lighting
                MaterialAmbientColor * ambient_light_strength +
                // Diffuse : "color" of the object
                MaterialDiffuseColor * (dir_diffuse_light + point_diffuse_light) +
                // Specular : reflective highlight, like a mirror
                MaterialSpecularColor * (dir_specular_light + point_specular_light);
                
            color.a = tex_rgba.a * opacity;
            
            if (color.a < 0.01f)
                discard;
        }
    )";

    return {vertexShader, fragmentShader};
}

Mesh::Mesh(
    const MeshData& p_data, 
    const Color& p_color,
    float p_diffuse_strength,
    float p_ambient_strength,
	float p_emissive_strength)
: verts(p_data.verts.transpose()), 
uvs(p_data.uvs.transpose()), 
norms(p_data.norms.transpose()),  
diffuse(p_color.getRGB()*p_diffuse_strength),
ambient(p_color.getRGB()*p_ambient_strength),
emissive(p_color.getRGB()*p_emissive_strength),
opacity(p_color.getAlpha())
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

Mesh::~Mesh()
{
}

void Mesh::fillInCommonProps(MeshSettings& settings)
{
    settings.pos_attr_loc = glGetAttribLocation(settings.program_id, "pos_in_model");
    settings.uv_attr_loc = glGetAttribLocation(settings.program_id, "in_uv");
    settings.norm_attr_loc = glGetAttribLocation(settings.program_id, "normal_in_model");

    settings.mvp_matrix_id = glGetUniformLocation(settings.program_id, "proj_from_model");
    settings.view_matrix_id = glGetUniformLocation(settings.program_id, "cam_from_world");
    settings.model_matrix_id = glGetUniformLocation(settings.program_id, "world_from_model");
    settings.modelview_matrix_id = glGetUniformLocation(settings.program_id, "cam_from_model");
    settings.diffuse_id = glGetUniformLocation(settings.program_id, "diffuse");
    settings.ambient_id = glGetUniformLocation(settings.program_id, "ambient");
    settings.emissive_id = glGetUniformLocation(settings.program_id, "emissive");
    settings.specular_id = glGetUniformLocation(settings.program_id, "specular");
    settings.light_ambient_id = glGetUniformLocation(settings.program_id, "ambient_light_strength");
    settings.light_pos_id = glGetUniformLocation(settings.program_id, "point_light_pos_in_world");
    settings.light_dir_id = glGetUniformLocation(settings.program_id, "dir_light_dir_in_world");
    settings.light_dir_strength_id = glGetUniformLocation(settings.program_id, "dir_light_strength");
    settings.light_point_strength_id = glGetUniformLocation(settings.program_id, "point_light_strength");
    settings.opacity_id = glGetUniformLocation(settings.program_id, "opacity");
    settings.tex_sampler_id  = glGetUniformLocation(settings.program_id, "tex_sampler");

    glGenVertexArrays(1, &settings.vao_id);
    glBindVertexArray(settings.vao_id);



    glBindBuffer(GL_ARRAY_BUFFER, pos_buffer_id);
    glVertexAttribPointer(
        settings.pos_attr_loc,                 
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    glEnableVertexAttribArray(settings.pos_attr_loc);

    
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_id);
    glVertexAttribPointer(
        settings.uv_attr_loc,                                // attribute
        2,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    glEnableVertexAttribArray(settings.uv_attr_loc);

    
    glBindBuffer(GL_ARRAY_BUFFER, norm_buffer_id);
    glVertexAttribPointer(
        settings.norm_attr_loc,                                // attribute
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    glEnableVertexAttribArray(settings.norm_attr_loc);

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

    glGenBuffers(1, &pos_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, pos_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &uv_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(GLfloat), uvs.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &norm_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, norm_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, norms.size() * sizeof(GLfloat), norms.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &tangent_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, tangent_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(GLfloat), tangents.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &bitangent_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, bitangent_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(GLfloat), bitangents.data(), GL_STATIC_DRAW);



    {
        auto mesh_shader = produceMeshShader(true);
        standard.program_id = buildProgram("mesh", mesh_shader.vertex, mesh_shader.fragment);

        fillInCommonProps(standard);

        standard.tangent_attr_loc = glGetAttribLocation(standard.program_id, "tangent_in_model");
        standard.bitangent_attr_loc = glGetAttribLocation(standard.program_id, "bitangent_in_model");

        standard.normal_sampler_id  = glGetUniformLocation(standard.program_id, "normal_sampler");

        glBindVertexArray(standard.vao_id);
        
        glBindBuffer(GL_ARRAY_BUFFER, tangent_buffer_id);
        glVertexAttribPointer(
            standard.tangent_attr_loc,                                // attribute
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );
        glEnableVertexAttribArray(standard.tangent_attr_loc);
        
        
        glBindBuffer(GL_ARRAY_BUFFER, bitangent_buffer_id);
        glVertexAttribPointer(
            standard.bitangent_attr_loc,                                // attribute
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );
        glEnableVertexAttribArray(standard.bitangent_attr_loc);

        glBindVertexArray(0);
    }

    {
        auto mesh_shader = produceMeshShader(false);
        no_normal_map.program_id = buildProgram("mesh_no_normal", mesh_shader.vertex, mesh_shader.fragment);

        fillInCommonProps(no_normal_map);
    }
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
        glDeleteVertexArrays(1, &standard.vao_id);
        glDeleteVertexArrays(1, &no_normal_map.vao_id);

        curr_window = NULL;
    }
}

void Mesh::renderOGLForSettings(Window* window, const TransformSim3& world_from_body, MeshSettings& settings)
{
    // Use our shader
    glUseProgram(settings.program_id);

    if (!tex)
    {
        tex = window->getDefaultTex();
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->getID());
    glUniform1i(settings.tex_sampler_id, 0);
	
    if (settings.normal_sampler_id != std::numeric_limits<GLuint>::max())
    {
        if (!normal_map)
	    {
            throw std::runtime_error("No normal map found in mesh.");
        }

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normal_map->getID());
        glUniform1i(settings.normal_sampler_id, 1);
    }


    Mat4 world_from_body_mat = world_from_body.matrix();

	Mat4 MV = window->getCamFromWorldMatrix() * world_from_body_mat;
    Mat4 MVP = window->getProjectionMatrix() * MV;

    glUniformMatrix4fv(settings.mvp_matrix_id, 1, GL_FALSE, MVP.data());
    glUniformMatrix4fv(settings.model_matrix_id, 1, GL_FALSE, world_from_body_mat.data());
    glUniformMatrix4fv(settings.view_matrix_id, 1, GL_FALSE, window->getCamFromWorldMatrix().data());
	glUniformMatrix4fv(settings.modelview_matrix_id, 1, GL_FALSE, MV.data());

    glUniform3fv(settings.diffuse_id, 1, diffuse.data());
    glUniform3fv(settings.ambient_id, 1, ambient.data());
	glUniform3fv(settings.emissive_id, 1, emissive.data());
    glUniform1f(settings.specular_id, specular);

	Vec3 ambient_light_strength = window->getAmbientLight();
	glUniform3fv(settings.light_ambient_id, 1, ambient_light_strength.data());

    for (const auto& point_light : window->getPointLights())
    {
        glUniform3fv(settings.light_pos_id, 1, point_light.second.pos.data());
        glUniform1f(settings.light_point_strength_id, point_light.second.strength);
        break;
    }

    auto dir_light = window->getDirLight();

	glUniform3fv(settings.light_dir_id, 1, dir_light.pos.data());
    glUniform1f(settings.light_dir_strength_id, dir_light.strength);

	glUniform1f(settings.opacity_id, opacity);

	glBindVertexArray(settings.vao_id);
	
	if (!cull)
		glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);

    glDrawArrays(GL_TRIANGLES, 0, verts.cols());

	if (!cull)
		glEnable(GL_CULL_FACE);
	
	glDisable(GL_BLEND);

    glBindVertexArray(0);
}

void Mesh::renderOGL(Window* window, const TransformSim3& world_from_body)
{
    if (normal_map)
        renderOGLForSettings(window, world_from_body, standard);
    else
        renderOGLForSettings(window, world_from_body, no_normal_map);
}

WrappedMesh::WrappedMesh(
    const shared_ptr<Mesh>& p_mesh,
    const Color& p_color,
    float p_diffuse_strength,
    float p_ambient_strength,
    float p_emissive_strength
)
: mesh(p_mesh), color(p_color),
    diffuse_strength(p_diffuse_strength),
    ambient_strength(p_ambient_strength),
    emissive_strength(p_emissive_strength)
{
}

void WrappedMesh::renderOGL(Window* window, const TransformSim3& world_from_body)
{
    mesh->setDiffuse(Color((Vec3)(color.getRGB() * diffuse_strength)));
    mesh->setAmbient(Color((Vec3)(color.getRGB() * ambient_strength)));
    mesh->setEmissive(Color((Vec3)(color.getRGB() * emissive_strength)));
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