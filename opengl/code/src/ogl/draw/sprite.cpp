/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#include "vephor/ogl/draw/sprite.h"

namespace vephor
{
namespace ogl
{

string spriteVertexShader = R"(
    #version 430
    layout(location = 0) in vec3 pos_in_model;
	layout(location = 1) in vec2 in_uv;
    
    uniform mat4 proj_from_camera;
	uniform mat4 cam_from_world;
	uniform mat4 world_from_cam;
    uniform mat4 world_from_model;
    uniform float scale;
	uniform vec3 point_light_pos_in_world;
    uniform vec3 dir_light_dir_in_world;

	out vec3 pos_in_world;
    out vec3 eye_dir_in_camera;
    out vec3 dir_light_dir_in_camera;
    out vec3 point_light_dir_in_camera;
	out vec2 uv;
    
    void main()
    {
		vec3 pos_in_camera = scale * pos_in_model + (cam_from_world * world_from_model * vec4(0,0,0,1)).xyz;
		pos_in_world = (world_from_cam * vec4(pos_in_camera, 1)).xyz;
		
        gl_Position = proj_from_camera * vec4(pos_in_camera, 1);
        
        // Vector that goes from the vertex to the camera, in camera space.
        // In camera space, the camera is at the origin (0,0,0).
        eye_dir_in_camera = vec3(0,0,0) - pos_in_camera;

        // Vector that goes from the vertex to the light, in camera space.
        vec3 point_light_pos_in_camera = ( cam_from_world * vec4(point_light_pos_in_world,1)).xyz;
        point_light_dir_in_camera = point_light_pos_in_camera + eye_dir_in_camera;

        dir_light_dir_in_camera = ( cam_from_world * vec4(dir_light_dir_in_world,0) ).xyz;

		uv = in_uv;
    }
)";

string spriteFragmentShader = R"(
	#version 430

	in vec3 pos_in_world;
    in vec3 eye_dir_in_camera;
    in vec3 dir_light_dir_in_camera;
    in vec3 point_light_dir_in_camera;
    in vec2 uv;

    out vec4 color;

	uniform vec2 uv_scale;
	uniform vec2 uv_offset;
	uniform vec3 diffuse;
    uniform vec3 ambient;
	uniform vec3 emissive;
	uniform vec3 point_light_pos_in_world;
	uniform float dir_light_strength = 0.0f;
    uniform float point_light_strength = 0.0f;
	uniform float opacity = 1.0f;
	uniform sampler2D tex_sampler;
	uniform sampler2D normal_sampler;

	void main()
	{
		// Light emission properties
        // You probably want to put them as uniforms
        vec3 point_light_color = vec3(1,1,1);
        vec3 dir_light_color = vec3(1,1,1);
        
        // Material properties
		vec2 final_uv = uv * uv_scale + uv_offset;
        vec4 tex_rgba = texture( tex_sampler, final_uv );
		
		vec3 normal_in_camera = texture( normal_sampler, final_uv ).xyz - vec3(0.5,0.5,0.5);
		normal_in_camera /= length(normal_in_camera);
		
		vec3 MaterialDiffuseColor = tex_rgba.xyz * diffuse;
        vec3 MaterialAmbientColor = tex_rgba.xyz * ambient;
		vec3 MaterialEmissiveColor = tex_rgba.xyz * emissive;
        vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

        // Distance to the light
        float distance = length( point_light_pos_in_world - pos_in_world );

        // Normal of the computed fragment, in camera space
        vec3 n = normalize( normal_in_camera );
        // Direction of the light (from the fragment to the light)
        vec3 l = normalize( point_light_dir_in_camera );
        // Cosine of the angle between the normal and the light direction, 
        // clamped above 0
        //  - light is at the vertical of the triangle -> 1
        //  - light is perpendicular to the triangle -> 0
        //  - light is behind the triangle -> 0
        float pointLightCosTheta = clamp( dot( n,l ), 0,1 );

        //float dirLightCosTheta = clamp( dot( n, vec3(0,0.099,-0.99) ), 0,1 );
		float dirLightCosTheta = clamp( dot( n, dir_light_dir_in_camera ), 0,1 );
        
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

        vec3 dir_specular_light = dir_light_color * dir_light_strength * pow(dir_specular_cos,5);
        vec3 point_specular_light = point_light_color * point_light_strength * pow(point_specular_cos,5) / (distance*distance);

        color.rgb = 
            // Ambient : simulates indirect lighting
            MaterialAmbientColor +
            // Diffuse : "color" of the object
            MaterialDiffuseColor * (dir_diffuse_light + point_diffuse_light) +
            // Specular : reflective highlight, like a mirror
            MaterialSpecularColor * (dir_specular_light + point_specular_light) + 
			MaterialEmissiveColor;
		color.a = tex_rgba.a * opacity;
		
		if (color.a < 0.01f)
			discard;
	}
)";

Sprite::Sprite(
    const shared_ptr<Texture>& p_sprite_sheet,
	Vec2i p_px_per_cell,
	bool x_flip,
	bool y_flip)
: sprite_sheet(p_sprite_sheet), 
diffuse(Vec3(1,1,1)),
ambient(Vec3(0.1,0.1,0.1))
{
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
	
	if (x_flip)
	{
		for (int i = 0; i < 6; i++)
		{
			verts(i,0) = -verts(i,0);
		}
		
		{
			Vec3 temp = verts.row(1);
			verts.row(1) = verts.row(2);
			verts.row(2) = temp;
		}
		
		{
			Vec2 temp = uvs.row(1);
			uvs.row(1) = uvs.row(2);
			uvs.row(2) = temp;
		}

		{
			Vec3 temp = verts.row(4);
			verts.row(4) = verts.row(5);
			verts.row(5) = temp;
		}
		
		{
			Vec2 temp = uvs.row(4);
			uvs.row(4) = uvs.row(5);
			uvs.row(5) = temp;
		}
	}
	
	if (y_flip)
	{
		for (int i = 0; i < 6; i++)
		{
			verts(i,1) = -verts(i,1);
		}
		
		{
			Vec3 temp = verts.row(1);
			verts.row(1) = verts.row(2);
			verts.row(2) = temp;
		}
		
		{
			Vec2 temp = uvs.row(1);
			uvs.row(1) = uvs.row(2);
			uvs.row(2) = temp;
		}

		{
			Vec3 temp = verts.row(4);
			verts.row(4) = verts.row(5);
			verts.row(5) = temp;
		}
		
		{
			Vec2 temp = uvs.row(4);
			uvs.row(4) = uvs.row(5);
			uvs.row(5) = temp;
		}
	}
	
	verts = verts.transpose().eval();
	uvs = uvs.transpose().eval();

	if (p_px_per_cell == Vec2i::Zero())
		p_px_per_cell = p_sprite_sheet->size();
	
	float aspect = (float)p_px_per_cell[0] / (float)p_px_per_cell[1];
	verts.row(0) *= aspect;
	
	// Form uvs for each cell in the sheet
	uv_scale = Vec2(p_px_per_cell[0] / (float)p_sprite_sheet->size()[0], p_px_per_cell[1] / (float)p_sprite_sheet->size()[1]);
	
	Vec2i cells(floor(1.0f / uv_scale[0]), floor(1.0f / uv_scale[1]));
	for (int y = 0; y < cells[1]; y++)
	{
		for (int x = 0; x < cells[0]; x++)
		{
			Vec2 start_px(x,y);
			uvs_per_cell.push_back(start_px.array() * uv_scale.array());
		}
	}

    if (uvs_per_cell.empty())
    {
        throw std::runtime_error("No cells found in sprite sheet.");
    }
}

Sprite::~Sprite()
{
}

void Sprite::onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
{
	program_id = buildProgram("sprite", spriteVertexShader, spriteFragmentShader);

    pos_attr_loc = glGetAttribLocation(program_id, "pos_in_model");
	uv_attr_loc = glGetAttribLocation(program_id, "in_uv");

    proj_matrix_id = glGetUniformLocation(program_id, "proj_from_camera");
	view_matrix_id = glGetUniformLocation(program_id, "cam_from_world");
	inv_view_matrix_id = glGetUniformLocation(program_id, "world_from_cam");
	model_matrix_id = glGetUniformLocation(program_id, "world_from_model");
    scale_id = glGetUniformLocation(program_id, "scale");
    diffuse_id = glGetUniformLocation(program_id, "diffuse");
    ambient_id = glGetUniformLocation(program_id, "ambient");
	emissive_id = glGetUniformLocation(program_id, "emissive");
    light_pos_id = glGetUniformLocation(program_id, "point_light_pos_in_world");
    light_dir_id = glGetUniformLocation(program_id, "dir_light_dir_in_world");
    light_dir_strength_id = glGetUniformLocation(program_id, "dir_light_strength");
    light_point_strength_id = glGetUniformLocation(program_id, "point_light_strength");
	opacity_id = glGetUniformLocation(program_id, "opacity");
	uv_scale_id = glGetUniformLocation(program_id, "uv_scale");
	uv_offset_id = glGetUniformLocation(program_id, "uv_offset");
	tex_sampler_id  = glGetUniformLocation(program_id, "tex_sampler");
	normal_sampler_id  = glGetUniformLocation(program_id, "normal_sampler");
	
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

	glGenBuffers(1, &pos_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, pos_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(
        pos_attr_loc,       
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
	glEnableVertexAttribArray(pos_attr_loc);

    glGenBuffers(1, &uv_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(GLfloat), uvs.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(
        uv_attr_loc,                                // attribute
        2,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
	glEnableVertexAttribArray(uv_attr_loc);

	glBindVertexArray(0);
}

void Sprite::onRemoveFromWindow(Window*)
{
	glDeleteBuffers(1, &pos_buffer_id);
    glDeleteBuffers(1, &uv_buffer_id);
    glDeleteVertexArrays(1, &vao_id);
}

void Sprite::renderOGL(Window* window, const TransformSim3& world_from_body)
{
    glUseProgram(program_id);

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite_sheet->getID());
    glUniform1i(tex_sampler_id, 0);

	if (!normal_sprite_sheet)
	{
		normal_sprite_sheet = window->getDefaultNormalMap();
	}
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normal_sprite_sheet->getID());
	glUniform1i(normal_sampler_id, 1);

	Mat4 world_from_body_mat = world_from_body.matrix();
	
	Mat4 cam_from_world_mat = window->getCamFromWorldMatrix();
	Mat4 world_from_cam_mat = cam_from_world_mat.inverse();

    Mat4 proj = window->getProjectionMatrix();

    glUniformMatrix4fv(proj_matrix_id, 1, GL_FALSE, proj.data());
    glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, world_from_body_mat.data());
    glUniformMatrix4fv(view_matrix_id, 1, GL_FALSE, cam_from_world_mat.data());
	glUniformMatrix4fv(inv_view_matrix_id, 1, GL_FALSE, world_from_cam_mat.data());

    glUniform3fv(diffuse_id, 1, diffuse.data());
    glUniform3fv(ambient_id, 1, ambient.data());
	glUniform3fv(emissive_id, 1, emissive.data());
	glUniform2fv(uv_scale_id, 1, uv_scale.data());
	glUniform2fv(uv_offset_id, 1, uvs_per_cell[curr_cell].data());

	for (const auto& point_light : window->getPointLights())
    {
        glUniform3fv(light_pos_id, 1, point_light.second.pos.data());
        glUniform1f(light_point_strength_id, point_light.second.strength);
        break;
    }

	auto dir_light = window->getDirLight();

	glUniform3fv(light_dir_id, 1, dir_light.pos.data());
    glUniform1f(light_dir_strength_id, dir_light.strength);
    glUniform1f(scale_id, world_from_body.scale);
	glUniform1f(opacity_id, opacity);
	
	glBindVertexArray(vao_id);

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);

    glDrawArrays(GL_TRIANGLES, 0, verts.cols());
	
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	glBindVertexArray(0);
}

} // namespace ogl
} // namespace vephor