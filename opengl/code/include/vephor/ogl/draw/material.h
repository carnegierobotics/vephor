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
    void setTexture(const shared_ptr<Texture>& p_tex){tex = p_tex;}
    shared_ptr<Texture> getTexture() const {return tex;}
	void setNormalMap(const shared_ptr<Texture>& p_normal_map){normal_map = p_normal_map;}
    shared_ptr<Texture> getNormalMap() const {return normal_map;}

	void setDiffuse(const Color& p_color){diffuse = p_color.getRGB();}
	Color getDiffuse() const {return diffuse;}
	void setAmbient(const Color& p_color){ambient = p_color.getRGB();}
	Color getAmbient() const {return ambient;}
    void setEmissive(const Color& p_color){emissive = p_color.getRGB();}
	Color getEmissive() const {return emissive;}
    void setSpecular(bool p_specular){specular = p_specular;}
    float getSpecular() const {return specular;}
	void setOpacity(const float& p_opacity){opacity = p_opacity;}
    float getOpacity() const {return opacity;}
    void setSize(const float& p_size){size = p_size;}
    float getSize() const {return size;}

    GLuint getPosAttrLoc() const {return pos_attr_loc;}
    GLuint getUVAttrLoc() const {return uv_attr_loc;}
    GLuint getNormAttrLoc() const {return norm_attr_loc;}
    GLuint getTangentAttrLoc() const {return tangent_attr_loc;}
    GLuint getBitangentAttrLoc() const {return bitangent_attr_loc;}
    GLuint getOffsetAttrLoc() const {return offset_attr_loc;}
    GLuint getColorAttrLoc() const {return color_attr_loc;}
    GLuint getSizeAttrLoc() const {return size_attr_loc;}

    void activate(Window* window, const TransformSim3& world_from_body);
    void deactivate();

    string getTag() const {return tag;}
private:
    friend MaterialBuilder;

    static const int MAX_NUM_POINT_LIGHTS = 4;

    string tag;
    GLuint program_id = std::numeric_limits<GLuint>::max();

    Vec3 diffuse = Vec3(1,1,1);
    Vec3 ambient = Vec3(1,1,1);
	Vec3 emissive = Vec3(0,0,0);
    float specular = 1.0f;
	float opacity = 1.0f;
    float size = 1.0f;

    shared_ptr<Texture> tex;
	shared_ptr<Texture> normal_map;

    GLuint pos_attr_loc = std::numeric_limits<GLuint>::max();
    GLuint uv_attr_loc = std::numeric_limits<GLuint>::max();
    GLuint norm_attr_loc = std::numeric_limits<GLuint>::max();
    GLuint tangent_attr_loc = std::numeric_limits<GLuint>::max();
    GLuint bitangent_attr_loc = std::numeric_limits<GLuint>::max();
    GLuint offset_attr_loc = std::numeric_limits<GLuint>::max();
    GLuint color_attr_loc = std::numeric_limits<GLuint>::max();
    GLuint size_attr_loc = std::numeric_limits<GLuint>::max();
    
    GLuint mvp_matrix_id = std::numeric_limits<GLuint>::max();
    GLuint proj_from_camera_id = std::numeric_limits<GLuint>::max();
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
    GLuint time_id = std::numeric_limits<GLuint>::max();
    GLuint size_id = std::numeric_limits<GLuint>::max();
    GLuint aspect_id = std::numeric_limits<GLuint>::max();
};

class MaterialBuilder
{
public:
    bool tex = false;
    bool normal_map = false;
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

    unordered_map<string, vector<string>> extra_sections;

    std::string produceVertexShader() const;
    std::string produceFragmentShader() const;
    std::string getTag() const;
    void saveShaders() const;
    std::shared_ptr<Material> build() const;
};

}