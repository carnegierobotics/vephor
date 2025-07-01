/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

#include "vephor.h"
#include "vephor_ogl.h"

using namespace vephor;

int main()
{
    Window window(-1, -1, "basic");

    window.setFrameLock(60.0f);
    window.setClearColor(Vec3(0.1,0.1,0.1));



    shared_ptr<Material> lighting_only_material;
    {
        MaterialBuilder builder;
        builder.tex = false;
        builder.normal_map = false;
        builder.dir_light = true;
        builder.point_lights = true;

        lighting_only_material = builder.build();
    }

    shared_ptr<Material> gradient_material;
    {
        MaterialBuilder builder;
        builder.tex = true;
        builder.normal_map = false;
        builder.dir_light = true;
        builder.point_lights = true;

        gradient_material = builder.build();
        auto gradient_img = generateGradientImage(Vec2i(64,64), Vec3(1,0,0), Vec3(0,0,1));
        gradient_material->setTexture(window.getTextureFromImage(*gradient_img, true));
        gradient_material->setOpacity(0.25);
    }

    shared_ptr<Material> checker_material;
    {
        MaterialBuilder builder;
        builder.tex = true;
        builder.normal_map = false;
        builder.dir_light = true;
        builder.point_lights = true;

        checker_material = builder.build();
        auto checker_img = generateCheckerboardImage(Vec2i(64,64), Vec2i(32,32), Vec3(0.33,0.33,0.33), Vec3(0.66,0.66,0.66));
        checker_material->setTexture(window.getTextureFromImage(*checker_img, true));
    }

    shared_ptr<Material> normal_only_material;
    {
        MaterialBuilder builder;
        builder.tex = true;
        builder.normal_map = true;
        builder.dir_light = true;
        builder.point_lights = true;

        auto normal_image = generateFlatNormalImage(Vec2i(128,128));
        auto circle_image = generateSimpleImage(Vec2i(128,128), Vec4(1,1,1,1));

        float rad = 45;
        float rad_spread = 10;
        float dist_spread = 5;
        for (int x = 0; x < normal_image->getSize()[0]; x++)
        {
            for (int y = 0; y < normal_image->getSize()[1]; y++)
            {
                Vec2 vec(x - normal_image->getSize()[0]/2, y - normal_image->getSize()[1]/2);
                float dist = vec.norm();
                if (dist >= rad - rad_spread && dist <= rad + rad_spread)
                {
                    float offset = sin((dist - rad) / rad_spread * M_PI);
                    float angle = offset * M_PI / 2;
                    Vec2 dir = vec / dist;
                    float s_ang = sin(angle);
                    (*normal_image)(x,y) = Vec3(s_ang*dir[0], s_ang*dir[1], cos(angle));
                }
                if (dist > rad + rad_spread)
                {
                    float peru = (dist - rad - rad_spread) / dist_spread;
                    (*circle_image)(x,y) = Vec4(255,255,255,(1-peru)*255).cast<uint8_t>();
                }
                if (dist > rad + rad_spread + dist_spread)
                {
                    (*circle_image)(x,y) = Vec4(255,255,255,0).cast<uint8_t>();
                }
            }
        }

        builder.saveShaders();
        normal_only_material = builder.build();
        auto normal_map = convertNormalImageToNormalMap(*normal_image);
        normal_only_material->setNormalMap(window.getTextureFromImage(*normal_map, false));

        normal_only_material->setTexture(window.getTextureFromImage(*circle_image, false));
    }

    shared_ptr<Material> time_varying_material;
    {
        MaterialBuilder builder;
        builder.tex = false;
        builder.normal_map = false;
        builder.dir_light = true;
        builder.point_lights = true;
        builder.time = true;

        builder.extra_sections["vertex_func"].push_back(R"(
            float wave(vec2 pos, float t) {
                return 0.3 * sin(2.0 * pos[0] + t * 5) * cos(2.0 * pos[1] + t * 5);
            }
        )");

        builder.extra_sections["vertex_main"].push_back(R"(
            curr_pos_in_model[2] += wave(curr_pos_in_model.xy, time);

            vec3 pos = curr_pos_in_model;
            
            float delta = 0.01;
            float hL = wave(pos.xy - vec2(delta, 0.0), time);
            float hR = wave(pos.xy + vec2(delta, 0.0), time);
            float hD = wave(pos.xy - vec2(0.0, delta), time);
            float hU = wave(pos.xy + vec2(0.0, delta), time);

            vec3 dx = normalize(vec3(2.0 * delta, 0.0, hR - hL));
            vec3 dy = normalize(vec3(0.0, 2.0 * delta, hU - hD));
            curr_normal_in_model = normalize(cross(dy, dx)); // dy × dx = normal
        )");

        time_varying_material = builder.build();
    }

    vector<shared_ptr<Material>> materials;
    materials.push_back(time_varying_material);
    materials.push_back(normal_only_material);
    materials.push_back(gradient_material);

    for (const auto& mat : materials)
    {
        v4print "Material tag:", mat->getTag();
    }

    int curr_material = 0;


    auto plane_mesh_data = formPlaneGrid(Vec2(10,10), Vec2i(100,100));

    auto mesh = make_shared<Mesh>(
        plane_mesh_data,
        materials[0]);

    auto mesh_render = window.add(mesh, TransformSim3(
		Vec3(0.0,0.0,0.0),
		Vec3(0.0,0.0,0.0),
		1.0f
	));


    {
        auto ground_plane_mesh_data = formPlane(Vec2(100,100));

        auto ground_mesh = make_shared<Mesh>(
            ground_plane_mesh_data,
            checker_material);

        window.add(ground_mesh, TransformSim3(
            Vec3(0.0,0.0,10.0),
            Vec3(0.0,0.0,0.0),
            1.0f
        ));
    }


    Vec3 light_dir(-1,-1,1);
    light_dir /= light_dir.norm();
    auto dir_light = make_shared<DirLight>(light_dir, 0.4f);
    window.add(dir_light, Transform3());

    const float point_light_height = -5.0;
    
    auto point_light = make_shared<PointLight>(5.0);
    auto point_light_render = window.add(point_light, Transform3(Vec3(10,0,point_light_height)));

    auto sphere_mesh_data = formSphere(16, 16);
    auto point_light_sphere = make_shared<Mesh>(
        sphere_mesh_data,
        Vec3(1.0,1.0,1.0),
        0.0, 0.0, 1.0);
    auto point_light_sphere_render = window.add(point_light_sphere, TransformSim3(
		Vec3(10.0,0.0,10.0),
		Vec3(0.0,0.0,0.0),
		0.5f
	));
	
    float t = 0;
    float last_switch = 0;
    const float material_switch_delta_s = 5.0;
    while (true)
    {
        if (t - last_switch > material_switch_delta_s)
        {
            last_switch = t;
            curr_material++;
            if (curr_material >= materials.size())
                curr_material = 0;
            mesh->setMaterial(materials[curr_material]);
        }

        point_light_render->setPos(Vec3(cos(t*2.5)*10.0,sin(t*2.5)*10.0,point_light_height));
        point_light_sphere_render->setPos(Vec3(cos(t*2.5)*10.0,sin(t*2.5)*10.0,point_light_height));

        Vec3 cam_focal_pt(0,0,0);
        float cam_angle = t;
        float cam_dist = 25.0;
        float cam_height = 12.0;
        Vec3 cam_pos = cam_focal_pt+Vec3(-sin(cam_angle)*cam_dist,-cos(cam_angle)*cam_dist,-cam_height);
            
        window.setCamFromWorld(makeLookAtTransform(
            cam_focal_pt,
            cam_pos,
            Vec3(0,0,-1)
        ));

        window.setCanonicalTime(t);

        if (!window.render())
            break;

        t += 0.015;
    }

    return 0;
}