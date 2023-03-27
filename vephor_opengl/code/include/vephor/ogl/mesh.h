#pragma once

#include "window.h"
#include "io.h"

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
	void setDiffuse(const Vec3& p_color){diffuse = p_color;}
	Vec3 getDiffuse() const {return diffuse;}
	void setAmbient(const Vec3& p_color){ambient = p_color;}
	Vec3 getAmbient() const {return ambient;}
	void setOpacity(const float& p_opacity){opacity = p_opacity;}
	void setCull(bool p_cull){cull = p_cull;}
    void setSpecular(bool p_specular){specular = p_specular;}
    void renderOGL(Window* window, const TransformSim3& world_from_body);
    void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node);
	void onRemoveFromWindow(Window* window);
private:
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
	
    GLuint vao_id;
    GLuint pos_buffer_id;
    GLuint uv_buffer_id;
    GLuint norm_buffer_id;
	GLuint tangent_buffer_id;
	GLuint bitangent_buffer_id;
    GLuint program_id;

    GLuint pos_attr_loc;
    GLuint uv_attr_loc;
    GLuint norm_attr_loc;
	GLuint tangent_attr_loc;
	GLuint bitangent_attr_loc;
    
    GLuint mvp_matrix_id;
	GLuint view_matrix_id;
	GLuint model_matrix_id;
	GLuint modelview_matrix_id;
    GLuint diffuse_id;
    GLuint ambient_id;
	GLuint emissive_id;
    GLuint specular_id;
	GLuint light_ambient_id;
    GLuint light_pos_id;
    GLuint light_dir_id;
    GLuint light_dir_strength_id;
    GLuint light_point_strength_id;
    GLuint opacity_id;
	GLuint tex_sampler_id;
	GLuint normal_sampler_id;
};

}