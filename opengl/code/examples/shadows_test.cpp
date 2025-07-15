/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#include "vephor.h"
#include "vephor_ogl.h"

using namespace vephor;
using namespace vephor::ogl;

int main()
{
    AssetManager asset_mgr;
    TrackballCamera cam_mgr;

    Window window(-1, -1, "basic", [&](Window* this_window, const Vec2i& window_size){
        cam_mgr.resizeWindow(*this_window);
	});

    float fps = 60.0f;
	window.setFrameLock(fps);
	float dt = 1.0f / fps;

    cam_mgr.setup({
		{"to", {0,0,0}},
		{"from", {50,-50,-50}},
		{"up", {0,0,-1}},
        {"3d", false}
	}, window, asset_mgr);

	BoundManager bound_mgr(&window);
	ControlInfo control_info;
    control_info.setStandardWindowCallbacks(window);



    for (int i = -250; i <= 250; i += 25)
    for (int j = -250; j <= 250; j += 25) 
    {
        auto mesh = make_shared<Sphere>();
        float i_peru = (i + 250) / 500.0f;
        float j_peru = (j + 250) / 500.0f;
        mesh->setColor(Vec3(i_peru,0,j_peru));
        auto mesh_render = window.add(mesh, TransformSim3(
            Vec3(i,j,-5.0),
            Vec3(0.0,0.0,0.0),
            1.0f
        ));
    }



    auto grid = make_shared<Grid>(250, Vec3(0,0,1), Vec3(1,0,0), 25.0f);
    window.add(grid);
    bound_mgr.addBoundSphere(150.0f, Transform3(Vec3(0,0,0)));


    auto plane = make_shared<Plane>(Vec2(250,250));
    plane->setColor(Vec3(0,0.5,0));
    window.add(plane, Transform3(Vec3(250,0,50.0)));
    bound_mgr.addBoundSphere(150.0f, Transform3(Vec3(250,0,50.0)));


    auto sphere = make_shared<Sphere>(20.0);
    sphere->setColor(Vec3(1,1,0));
    auto sphere_node = window.add(sphere, Transform3(Vec3(350,0,0)));

    auto sphere_2 = make_shared<Sphere>(15.0);
    sphere_2->setColor(Vec3(1,0,1));
    auto sphere_2_node = window.add(sphere_2, Transform3(Vec3(350,0,-50)));

    auto sphere_3 = make_shared<Sphere>(10.0);
    sphere_3->setColor(Vec3(0,1,1));
    auto sphere_3_node = window.add(sphere_3, Transform3(Vec3(350,0,-100)));

    auto cube = make_shared<Cube>(10.0);
    cube->setColor(Vec3(1,0,0));
    auto cube_node = window.add(cube, Transform3(Vec3(425,125,0)));



    Material time_varying_material;
    {
        MaterialBuilder builder;
        builder.tex = false;
        builder.normal_map = false;
        builder.dir_light = true;
        builder.point_lights = true;
        builder.time = true;

        builder.extra_sections["vertex_main"].push_back(R"(
            curr_pos_in_model[0] *= (0.6 + 0.4 * sin(time * 3 + curr_pos_in_model[1]*0.25));
        )");

        time_varying_material.set = builder.buildSet();
    }

    auto plane_mesh_data = formPlaneGrid(Vec2(30,100), Vec2i(100,100));

    {
        auto mesh = make_shared<Mesh>(
            plane_mesh_data,
            time_varying_material);

        auto mesh_render = window.add(mesh, TransformSim3(
            Vec3(425,-125,0),
            Vec3(0.0,0.0,0.0),
            1.0f
        ));
    }

    {
        auto mesh = make_shared<Mesh>(
            plane_mesh_data,
            time_varying_material);

        auto mesh_render = window.add(mesh, TransformSim3(
            Vec3(425,-125,-150),
            Vec3(0.0,0.0,M_PI / 2),
            1.0f
        ));
    }

    

    Vec3 light_dir(0,0,1);
    light_dir /= light_dir.norm();
    auto dir_light = make_shared<DirLight>(light_dir, 0.6f);
    dir_light->enableShadows();
    window.add(dir_light, Transform3());

    //auto ambient_light = make_shared<AmbientLight>(Vec3(0.6f,0.6f,0.6f));
    //window.add(ambient_light);

    Mat4 proj = makePerspectiveProj(45, window.getSize(), 0.1f, 100.0f);
    window.setProjectionMatrix(proj);

    cam_mgr.autoFitPoints(window, bound_mgr.calcCameraBoundPoints());

    float t = 0;
    while (window.render())
    {
        cam_mgr.update(window, dt, control_info);
		control_info.onUpdate();

        sphere_node->setPos(Vec3(350,sin(t)*200,0));
        sphere_2_node->setPos(Vec3(350,sin(t+2*M_PI/3)*200,-50));
        sphere_3_node->setPos(Vec3(350,sin(t+4*M_PI/3)*200,-100));

        cube_node->setOrient(Vec3(sin(t),sin(t)*5,sin(t)*13));

        window.setCanonicalTime(t);

        t += dt;
    }

    return 0;
}