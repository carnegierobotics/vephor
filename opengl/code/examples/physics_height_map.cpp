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
#include "vephor/verlet.h"
#include <random>

using namespace vephor;

int main()
{
    Window window(-1, -1, "physics height map");

	Verlet phys(30.0);

    window.setFrameLock(60.0f);

	auto cube_mesh_data = formCube();
	auto sphere_mesh_data = formSphere(16, 16);

#if 1
	MatX heights(65,65);
	heights.fill(0);

	for (int i = 0; i < 65; i++)
    {
        for (int j = 0; j < 65; j++)
        {
            heights(i,j) = pow(1.0 - j / 64.0f, 2.0f) * 64.0f - 32;
			
			if (heights(i,j) < -20)
				heights(i,j) = -20;
			
			if (j > 60)
				heights(i,j) = 0;
        }
    }

    /*for (int i = 32-6; i <= 32+6; i++)
		for (int j = 24; j <= 32; j++)
			heights(i,j) = 2.5;

	for (int i = 32-5; i <= 32+5; i++)
		for (int j = 25; j <= 31; j++)
			heights(i,j) = 5.0;

	for (int i = 32-3; i <= 32+3; i++)
		for (int j = 27; j <= 29; j++)
			heights(i,j) = 3.0;

    for (int i = 0; i < 64; i++)
    {
        for (int j = 40; j < 64; j++)
        {
            heights(i,j) = pow((j - 40) * 0.2, 2.0);
        }
    }

    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            heights(i,j) = -pow((20 - j) * 0.2, 2.0);
        }
    }*/
	
	float height_map_res = 1.0f;
#else
	MatX heights(2,2);
	heights.fill(0);
	
	heights(0,1) = 3;
	heights(1,0) = 3;

	float height_map_res = 64.0f;
#endif

	auto hm_mesh_data = formHeightMap(heights, height_map_res);

	auto hm_mesh = make_shared<Mesh>(
        hm_mesh_data,
        Vec3(1.0,1.0,1.0));

    auto hm_tex = window.getTextureFromImage(*generateCheckerboardImage(Vec2i(256,256), Vec2i(16,16), Vec3(1.0,0.0,0.0), Vec3(0.0,0.0,1.0)));
    hm_mesh->setTexture(hm_tex);

    auto hm_mesh_render = window.add(hm_mesh, Transform3(
        Vec3(0.0,0.0,20.0),
		Vec3(0.0,0.0,0.0)
	));
	
	
	/*auto solids = createSolidsFromHeightMap(heights, 1.0);
	for (const auto solid : solids)
	{
		auto hm_solid_obj = make_shared<Simple>(solid.first + hm_mesh->getPos());
		phys.add(hm_solid_obj, Verlet::makeSolid(solid.second), 0.0f);
	}*/
	
	phys.add(hm_mesh_render, Verlet::makeHeightMap(heights, height_map_res), 0.0f);
	

    auto mesh_3 = make_shared<Mesh>(
        sphere_mesh_data,
        Vec3(1.0,1.0,1.0),
        0.0, 0.0, 1.0);
    auto mesh_3_render = window.add(mesh_3, TransformSim3(
		Vec3(10.0,0.0,10.0),
		Vec3(0.0,0.0,0.0),
		0.5
	));

    auto point_light = make_shared<PointLight>();
    auto point_light_render = window.add(point_light, Transform3(Vec3(10,0,10)));

    Vec3 light_dir(-1,-1,1);
    light_dir /= light_dir.norm();
    auto dir_light = make_shared<DirLight>(light_dir, 0.4f);
    window.add(dir_light, Transform3());
	
	std::default_random_engine generator;
    std::uniform_real_distribution<double> x_dist(-32.0,32.0);
	std::uniform_real_distribution<double> y_dist(-20.0,-12.0);

	for (int s = 0; s < 50; s++)
	{
		auto sphere_obj = make_shared<Mesh>(
			sphere_mesh_data,
			Vec3(0.25,0.5,0.5));
		auto sphere_obj_render = window.add(sphere_obj, Transform3(
			Vec3(x_dist(generator),y_dist(generator),0.0),
			Vec3(0.0,0.0,0.0)
		));

		phys.add(sphere_obj_render, Verlet::makeSphere(1.0f), 5.0f);
	}

    Mat4 proj = makePerspectiveProj(45, window.getSize(), 0.1f, 1000.0f);
    window.setProjectionMatrix(proj);

    window.setCamFromWorld(makeLookAtTransform(
        Vec3(0,0,20),
        Vec3(-90,0,17),
        Vec3(0,0,-1)
    ));

    float t = 0;
	float dt = 1.0/60.0;
    while (window.render())
    {
		phys.update(dt);
		
        point_light_render->setPos(Vec3(cos(t*2.5)*10.0,sin(t*2.5)*10.0,10.0));
        mesh_3_render->setPos(Vec3(cos(t*2.5)*10.0,sin(t*2.5)*10.0,10.0));

        /*Vec3 new_light_dir(cos(t), sin(t), 1.0);
        new_light_dir /= new_light_dir.norm();
        dir_light->setDir(new_light_dir);*/

        /*Vec3 cam_focal_pt(0,0,8);
        float cam_angle = t;
        float cam_dist = 25.0;
        float cam_height = 12.0;
        Vec3 cam_pos = cam_focal_pt+Vec3(-sin(cam_angle)*cam_dist,-cos(cam_angle)*cam_dist,-cam_height);
            
        window.setCamFromWorld(makeLookAtTransform(
            cam_focal_pt,
            cam_pos,
            Vec3(0,0,-1)
        ));*/

        t += dt;
    }

    return 0;
}