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
    AssetManager asset_mgr;
    TrackballCamera cam_mgr;

    Window window(-1, -1, "basic");

    float fps = 60.0f;
	window.setFrameLock(fps);
	float dt = 1.0f / fps;

    cam_mgr.setup({
		{"to", {0,0,0}},
		{"from", {-50,0,0}},
		{"up", {0,0,-1}},
        {"3d", false}
	}, window, asset_mgr);

	BoundManager bound_mgr(&window);
	ControlInfo control_info;
    control_info.setStandardWindowCallbacks(window);


    Vec3 normal(5,2,3);
    normal /= normal.norm();

    float plane_dist = 120;

    Vec4 plane = Vec4(0,0,0,-plane_dist);
    plane.head<3>() = normal;
    
    Vec3 plane_pt = normal*-plane[3];
    
    
    Mat4 reflect_mat = reflectMatrix(plane);


    Transform3 world_from_cam(Vec3(40,10,0), Vec3(0,0,M_PI/6));


    for (int i = -100; i <= 100; i += 25)
    for (int j = -100; j <= 100; j += 25) 
    {
        float i_peru = (i + 100) / 200.0f;
        float j_peru = (j + 100) / 200.0f;

        {
            auto mesh = make_shared<Sphere>();    
            mesh->setColor(Vec3(i_peru,0,j_peru));
            auto mesh_render = window.add(mesh, TransformSim3(
                Vec3(i,j,-2.0),
                Vec3(0.0,0.0,0.0),
                1.0f
            ));
        }

        {
            auto mesh = make_shared<Sphere>();
            mesh->setColor(Vec3(i_peru,j_peru,i_peru*0.5));
            auto mesh_render = window.add(mesh, TransformSim3(
                Vec3(i,j,-50.0),
                Vec3(0.0,0.0,0.0),
                1.0f
            ));
        }
    }


    {
        auto grid = make_shared<Grid>(100, Vec3(0,0,1), Vec3(1,0,0), 25.0f);
        window.add(grid);
        bound_mgr.addBoundSphere(10.0f, Transform3(Vec3(0,0,0)));
    }

    


    {
        Mat4 world_M_cam = world_from_cam.matrix();
        Vec3 world_t_cam = world_M_cam.block(0,3,3,1);
        Mat3 world_R_cam = world_M_cam.block<3,3>(0,0);

        v4print "Normal:";
        v4print world_M_cam;
        v4print world_t_cam.transpose();

        for (int i = 0; i < 3; i++)
        {
            auto arrow = make_shared<Arrow>(world_t_cam, world_t_cam+world_R_cam.col(i)*5);
            Vec3 color = Vec3::Zero();
            color[i] = 1;
            arrow->setColor(color);
            window.add(arrow);
        }
    }

    {
        Mat4 world_M_cam = reflect_mat * world_from_cam.matrix();
        Vec3 world_t_cam = world_M_cam.block(0,3,3,1);
        Mat3 world_R_cam = world_M_cam.block<3,3>(0,0);

        v4print "Reflected:";
        v4print world_M_cam;
        v4print world_t_cam.transpose();

        for (int i = 0; i < 3; i++)
        {
            auto arrow = make_shared<Arrow>(world_t_cam, world_t_cam+world_R_cam.col(i)*5);
            Vec3 color = Vec3::Zero();
            color[i] = 1;
            arrow->setColor(color);
            window.add(arrow);
        }
    }




    {
        auto mirror_tex = window.addReflectiveSurface(plane);

        auto plane_data = formPlaneGrid(Vec2(100,100), Vec2i(100,100));

        MaterialBuilder builder;
        builder.dir_light = true;
        builder.point_lights = true;
        builder.tex = true;
        builder.normal_map = false;
        builder.screen_space_tex_coords = true;
        builder.time = true;

        builder.extra_sections["vertex_func"].push_back(R"(
            float wave(vec2 pos, float t) {
                return 1.5 * sin(0.3 * pos[0] + t * 5) * cos(0.3 * pos[1] + t * 5);
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

        builder.extra_sections["frag_main"].push_back(R"(
            // Fresnel factor using Schlick's approximation
            float fresnel = pow(1.0 - max(abs(dot(E, n)), 0.0), 5.0);
            fresnel = mix(0.25, 1.0, fresnel);  // 0.04 is base reflectivity

            vec4 surface_color = vec4(0.0, 0.3, 0.4, 1);
            tex_rgba = mix(surface_color, tex_rgba, fresnel);
        )");

        auto material = builder.build();

        material->setTexture(mirror_tex);

		auto plane_mesh = make_shared<Mesh>(plane_data, material);
        window.add(plane_mesh, Transform3(plane_pt, Orient3::rotateTo(Vec3(0,0,1), normal)));
        bound_mgr.addBoundSphere(10.0f, Transform3(plane_pt));
    }




    {
        Vec3 normal(0,0,1);
        normal /= normal.norm();

        float plane_dist = -75;

        Vec4 plane = Vec4(0,0,0,-plane_dist);
        plane.head<3>() = normal;

        Vec3 plane_pt = normal*-plane[3];


        auto mirror_tex = window.addReflectiveSurface(plane);

        auto plane_data = formPlaneGrid(Vec2(100,100), Vec2i(100,100));

        MaterialBuilder builder;
        builder.dir_light = true;
        builder.point_lights = true;
        builder.tex = true;
        builder.normal_map = false;
        builder.screen_space_tex_coords = true;
        builder.time = true;

        builder.extra_sections["vertex_func"].push_back(R"(
            float wave(vec2 pos, float t) {
                return 1.5 * sin(0.3 * pos[0] + t * 5) * cos(0.3 * pos[1] + t * 5);
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

        builder.extra_sections["frag_main"].push_back(R"(
            // Fresnel factor using Schlick's approximation
            float fresnel = pow(1.0 - max(abs(dot(E, n)), 0.0), 5.0);
            fresnel = mix(0.25, 1.0, fresnel);  // 0.04 is base reflectivity

            vec4 surface_color = vec4(0.0, 0.3, 0.4, 1);
            tex_rgba = mix(surface_color, tex_rgba, fresnel);
        )");

        auto material = builder.build();

        material->setTexture(mirror_tex);

		auto plane_mesh = make_shared<Mesh>(plane_data, material);
        window.add(plane_mesh, Transform3(plane_pt, Orient3::rotateTo(Vec3(0,0,1), normal)));
        bound_mgr.addBoundSphere(10.0f, Transform3(plane_pt));
    }





    Vec3 light_dir(-1,-1,1);
    light_dir /= light_dir.norm();
    auto dir_light = make_shared<DirLight>(light_dir, 0.4f);
    window.add(dir_light, Transform3());

    auto ambient_light = make_shared<AmbientLight>(Vec3(0.6f,0.6f,0.6f));
    window.add(ambient_light);

    Mat4 proj = makePerspectiveProj(45, window.getSize(), 0.1f, 100.0f);
    window.setProjectionMatrix(proj);

    cam_mgr.autoFitPoints(window, bound_mgr.calcCameraBoundPoints());

    float t = 0;
    while (window.render())
    {
        cam_mgr.update(window, dt, control_info);
		control_info.onUpdate();

        window.setCanonicalTime(t);

        t += dt;
    }

    return 0;
}