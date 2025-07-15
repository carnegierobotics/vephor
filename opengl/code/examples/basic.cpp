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
    Window window(-1, -1, "basic");

    window.setFrameLock(60.0f);
	
	auto pyramid_mesh_parent = window.add(TransformSim3(Vec3(0,0,8), Vec3::Zero(), 3.0f));

    {
		auto mesh_parts = loadOBJMesh("../src/assets/pyramid.obj");
		for (const auto& part : mesh_parts)
		{
			auto curr_mesh_obj = make_shared<Mesh>(
				part.geometry,
				part.diffuse);
			window.add(curr_mesh_obj)->setParent(pyramid_mesh_parent);
		}
    }

	auto cube_mesh_data = formCube();
	auto sphere_mesh_data = formSphere(16, 16);

	MatX heights(64,64);
	heights.fill(0);

    for (int i = 32-6; i <= 32+6; i++)
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
    }

	auto hm_mesh_data = formHeightMap(heights, 1.0);

	auto hm_mesh = make_shared<Mesh>(
        hm_mesh_data,
        Vec3(1.0,1.0,1.0));

    auto hm_tex = window.getTextureFromImage(*generateCheckerboardImage(Vec2i(256,256), Vec2i(16,16), Vec3(1.0,0.0,0.0), Vec3(0.0,0.0,1.0)));
    hm_mesh->setTexture(hm_tex);

    window.add(hm_mesh, Transform3(
		Vec3(0.0,0.0,20.0),
		Vec3(0.0,0.0,0.0)
	));

    auto mesh_2 = make_shared<Mesh>(
        cube_mesh_data,
        Vec3(1.0,1.0,0.0));
    auto mesh_2_render = window.add(mesh_2, Transform3(
		Vec3(0.0,0.0,12.0),
		Vec3(0.0,0.0,0.0)
	));

    auto mesh_3 = make_shared<Mesh>(
        sphere_mesh_data,
        Vec3(1.0,1.0,1.0),
        0.0, 0.0, 1.0);
    auto mesh_3_render = window.add(mesh_3, TransformSim3(
		Vec3(10.0,0.0,10.0),
		Vec3(0.0,0.0,0.0),
		0.5f
	));

    auto point_light = make_shared<PointLight>();
    auto point_light_render = window.add(point_light, Transform3(Vec3(10,0,10)));

    Vec3 light_dir(-1,-1,1);
    light_dir /= light_dir.norm();
    auto dir_light = make_shared<DirLight>(light_dir, 0.4f);
    window.add(dir_light, Transform3());

    Mat4 proj = makePerspectiveProj(45, window.getSize(), 0.1f, 100.0f);
    window.setProjectionMatrix(proj);

    window.setCamFromWorld(makeLookAtTransform(
        Vec3(0,0,12),
        Vec3(-12,0,0),
        Vec3(0,0,-1)
    ));

    float t = 0;
    while (window.render())
    {
        mesh_2_render->setPos(Vec3(cos(t)*5.0,sin(t)*5.0,12.0));
        mesh_2_render->setOrient(mesh_2_render->getOrient() * Orient3(Vec3(0,0,0.01)));

        point_light_render->setPos(Vec3(cos(t*2.5)*10.0,sin(t*2.5)*10.0,10.0));
        mesh_3_render->setPos(Vec3(cos(t*2.5)*10.0,sin(t*2.5)*10.0,10.0));

        Vec3 cam_focal_pt(0,0,8);
        float cam_angle = t;
        float cam_dist = 25.0;
        float cam_height = 12.0;
        Vec3 cam_pos = cam_focal_pt+Vec3(-sin(cam_angle)*cam_dist,-cos(cam_angle)*cam_dist,-cam_height);
            
        window.setCamFromWorld(makeLookAtTransform(
            cam_focal_pt,
            cam_pos,
            Vec3(0,0,-1)
        ));



        t += 0.015;
    }

    return 0;
}