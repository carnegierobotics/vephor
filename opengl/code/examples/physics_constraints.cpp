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
using namespace vephor::ogl;

int main()
{
    Window window(-1, -1, "physics");
    Verlet phys(30.0);

    window.setFrameLock(60.0f);

	auto sphere_mesh_data = formSphere(16, 16);

    MatX plane_verts(2,3);
    plane_verts <<
        0,-250,0,
        0,250,0;

    const int sphere_n_angles = 36;
    MatX sphere_verts(sphere_n_angles*2,3);
    for (int i = 0; i < sphere_n_angles; i++)
    {
        float angle = (float)i / sphere_n_angles * (2*M_PI);
        float next_angle = (float)(i+1) / sphere_n_angles * (2*M_PI);

        float c_angle = cos(angle);
        float s_angle = sin(angle);
        float c_next_angle = cos(next_angle);
        float s_next_angle = sin(next_angle);

        sphere_verts.row(i*2+0) = Vec3(0, s_angle, c_angle);
        sphere_verts.row(i*2+1) = Vec3(0, s_next_angle, c_next_angle);
    }

    auto plane_1 = make_shared<Lines>(
        plane_verts,
		MatX(),
        Vec3(1,1,1)
    );
    auto plane_1_render = window.add(plane_1, Transform3(
		Vec3(0.0,-6.0,0.0),
		Vec3(M_PI/4.0,0.0,0.0)
	));

    auto plane_2 = make_shared<Lines>(
        plane_verts,
		MatX(),
        Vec3(1,1,1)
    );
    auto plane_2_render = window.add(plane_2, Transform3());

    auto plane_3 = make_shared<Lines>(
        plane_verts,
		MatX(),
        Vec3(1.0,1.0,1.0));
    auto plane_3_render = window.add(plane_3, Transform3(
		Vec3(0.0,6.0,0.0),
		Vec3(-M_PI/4.0,0.0,0.0)
	));

    Vec3 light_dir(-1,-1,1);
    light_dir /= light_dir.norm();
    auto dir_light = make_shared<DirLight>(light_dir, 0.7f);
    window.add(dir_light, Transform3());

    //Mat4 proj = makePerspectiveProj(45, window.getSize(), 0.1f, 100.0f);
    float aspect = (float)window.getSize()[0]/window.getSize()[1];
    float zoom = 40.0f;
    Mat4 proj = makeOrthoProj(
        Vec3(-zoom*aspect,-zoom,-zoom), 
        Vec3(zoom*aspect,zoom,zoom)
    );
    window.setProjectionMatrix(proj);

    window.setCamFromWorld(makeLookAtTransform(
        Vec3(0,0,-40.0f),
        Vec3(-100,0,-40.0f),
        Vec3(0,0,-1)
    ));

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(-100.0,100.0);

    int n_boxes = 20;
    for (int i = 0; i < n_boxes; i++)
    {
        Vec3 center(0, distribution(generator), -100.0+distribution(generator));

        const float sphere_size = 1.25f;
        const float box_side = 5.0f;
        const float h_box_side = box_side / 2.0f;
        float perc = (float)i/(n_boxes-1);
        Vec3 color(perc,(perc-0.5)*(perc-0.5)*4.0,1.0-perc);

        v4print i, color.transpose(), center.transpose();

        auto sphere_00_obj = make_shared<Mesh>(
            sphere_mesh_data,
            color);
        auto sphere_00_obj_render = window.add(sphere_00_obj, TransformSim3(
			center + Vec3(0.0,-h_box_side,-h_box_side),
			Vec3(0.0,0.0,0.0),
			sphere_size
		));
        auto phys_sphere_00_obj = phys.add(sphere_00_obj_render, Verlet::makeSphere(1.0f), 1.0f);

        auto sphere_10_obj = make_shared<Mesh>(
            sphere_mesh_data,
            color);
        auto sphere_10_obj_render = window.add(sphere_10_obj, TransformSim3(
			center + Vec3(0.0,h_box_side,-h_box_side),
			Vec3(0.0,0.0,0.0),
			sphere_size
		));
        auto phys_sphere_10_obj = phys.add(sphere_10_obj_render, Verlet::makeSphere(1.0f), 1.0f);

        auto sphere_01_obj = make_shared<Mesh>(
            sphere_mesh_data,
            color);
        auto sphere_01_obj_render = window.add(sphere_01_obj, TransformSim3(
			center + Vec3(0.0,-h_box_side,h_box_side),
			Vec3(0.0,0.0,0.0),
			sphere_size
		));
        auto phys_sphere_01_obj = phys.add(sphere_01_obj_render, Verlet::makeSphere(1.0f), 1.0f);

        auto sphere_11_obj = make_shared<Mesh>(
            sphere_mesh_data,
            color);
        auto sphere_11_obj_render = window.add(sphere_11_obj, TransformSim3(
			center + Vec3(0.0,h_box_side,h_box_side),
			Vec3(0.0,0.0,0.0),
			sphere_size
		));
        auto phys_sphere_11_obj = phys.add(sphere_11_obj_render, Verlet::makeSphere(1.0f), 1.0f);


        auto sphere_side_y0_obj = make_shared<Mesh>(
            sphere_mesh_data,
            color);
        auto sphere_side_y0_obj_render = window.add(sphere_side_y0_obj, TransformSim3(
			center + Vec3(0.0,-h_box_side,0.0),
			Vec3(0.0,0.0,0.0),
			sphere_size
		));
        auto phys_sphere_side_y0_obj = phys.add(sphere_side_y0_obj_render, Verlet::makeSphere(1.0f), 1.0f);

        auto sphere_side_y1_obj = make_shared<Mesh>(
            sphere_mesh_data,
            color);
        auto sphere_side_y1_obj_render = window.add(sphere_side_y1_obj, TransformSim3(
			center + Vec3(0.0,h_box_side,0.0),
			Vec3(0.0,0.0,0.0),
			sphere_size
		));
        auto phys_sphere_side_y1_obj = phys.add(sphere_side_y1_obj_render, Verlet::makeSphere(1.0f), 1.0f);


        auto sphere_side_z0_obj = make_shared<Mesh>(
            sphere_mesh_data,
            color);
        auto sphere_side_z0_obj_render = window.add(sphere_side_z0_obj, TransformSim3(
			center + Vec3(0.0,0.0,-h_box_side),
			Vec3(0.0,0.0,0.0),
			sphere_size
		));
        auto phys_sphere_side_z0_obj = phys.add(sphere_side_z0_obj_render, Verlet::makeSphere(1.0f), 1.0f);

        auto sphere_side_z1_obj = make_shared<Mesh>(
            sphere_mesh_data,
            color);
        auto sphere_side_z1_obj_render = window.add(sphere_side_z1_obj, TransformSim3(
			center + Vec3(0.0,0.0,h_box_side),
			Vec3(0.0,0.0,0.0),
			sphere_size
		));
        auto phys_sphere_side_z1_obj = phys.add(sphere_side_z1_obj_render, Verlet::makeSphere(1.0f), 1.0f);


        phys.addConstraint(phys_sphere_00_obj, phys_sphere_10_obj, box_side);
        phys.addConstraint(phys_sphere_00_obj, phys_sphere_01_obj, box_side);
        phys.addConstraint(phys_sphere_10_obj, phys_sphere_01_obj, box_side * sqrt(2.0f));
        phys.addConstraint(phys_sphere_11_obj, phys_sphere_10_obj, box_side);
        phys.addConstraint(phys_sphere_11_obj, phys_sphere_01_obj, box_side);

        phys.addConstraint(phys_sphere_00_obj, phys_sphere_side_y0_obj, h_box_side);
        phys.addConstraint(phys_sphere_01_obj, phys_sphere_side_y0_obj, h_box_side);

        phys.addConstraint(phys_sphere_10_obj, phys_sphere_side_y1_obj, h_box_side);
        phys.addConstraint(phys_sphere_11_obj, phys_sphere_side_y1_obj, h_box_side);

        phys.addConstraint(phys_sphere_00_obj, phys_sphere_side_z0_obj, h_box_side);
        phys.addConstraint(phys_sphere_10_obj, phys_sphere_side_z0_obj, h_box_side);

        phys.addConstraint(phys_sphere_01_obj, phys_sphere_side_z1_obj, h_box_side);
        phys.addConstraint(phys_sphere_11_obj, phys_sphere_side_z1_obj, h_box_side);

        //phys.addConstraint(phys_sphere_side_y0_obj, phys_sphere_side_y1_obj, 10.0f);
        //phys.addConstraint(phys_sphere_side_z0_obj, phys_sphere_side_z1_obj, 10.0f);
    }


    {
        const int n_rope_links = 40;
        const float rope_sphere_size = 0.5f;
        const float rope_link_length = 1.0f;
        const Vec3 rope_color(0.5,0.5,0.5);

        auto rope_root_obj = make_shared<Mesh>(
            sphere_mesh_data,
            rope_color);
        auto rope_root_obj_render = window.add(rope_root_obj, TransformSim3(
			Vec3(0,0,-50),
			Vec3(0.0,0.0,0.0),
			rope_sphere_size
		));
        auto last_phys_rope_obj = phys.add(rope_root_obj_render, Verlet::makeSphere(1.0f), 0.0f);

        for (int i = 0; i < n_rope_links; i++)
        {
            auto rope_obj = make_shared<Mesh>(
                sphere_mesh_data,
                rope_color);
            auto rope_obj_render = window.add(rope_obj, TransformSim3(
				last_phys_rope_obj->getPos() + Vec3(0,1,0)*rope_link_length,
				Vec3(0.0,0.0,0.0),
				rope_sphere_size
			));
            auto phys_rope_obj = phys.add(rope_obj_render, Verlet::makeSphere(1.0f), 1.0f);

            phys.addConstraint(phys_rope_obj, last_phys_rope_obj, rope_link_length);

            last_phys_rope_obj = phys_rope_obj;
        }
    }


    {
        const int n_rope_links = 30;
        const float rope_sphere_size = 0.5f;
        const float rope_link_length = 1.0f;
        const Vec3 rope_color(0.5,0.5,0.5);

        auto rope_root_obj = make_shared<Mesh>(
            sphere_mesh_data,
            rope_color);
        auto rope_root_obj_render = window.add(rope_root_obj, TransformSim3(
			Vec3(0,-20,-40),
			Vec3(0.0,0.0,0.0),
			rope_sphere_size
		));
        auto last_phys_rope_obj = phys.add(rope_root_obj_render, Verlet::makeSphere(1.0f), 0.0f);

        for (int i = 0; i < n_rope_links; i++)
        {
            auto rope_obj = make_shared<Mesh>(
                sphere_mesh_data,
                rope_color);
            auto rope_obj_render = window.add(rope_obj, TransformSim3(
				last_phys_rope_obj->getPos() + Vec3(0,1,0)*rope_link_length,
				Vec3(0.0,0.0,0.0),
				rope_sphere_size
			));
            auto phys_rope_obj = phys.add(rope_obj_render, Verlet::makeSphere(1.0f), 1.0f);

            phys.addConstraint(phys_rope_obj, last_phys_rope_obj, rope_link_length);

            last_phys_rope_obj = phys_rope_obj;
        }
    }

    {
        const int n_rope_links = 50;
        const float rope_sphere_size = 0.5f;
        const float rope_link_length = 1.0f;
        const Vec3 rope_color(0.5,0.5,0.5);

        auto rope_root_obj = make_shared<Mesh>(
            sphere_mesh_data,
            rope_color);
        auto rope_root_obj_render = window.add(rope_root_obj, TransformSim3(
			Vec3(0,20,-60),
			Vec3(0.0,0.0,0.0),
			rope_sphere_size
		));
        auto last_phys_rope_obj = phys.add(rope_root_obj_render, Verlet::makeSphere(1.0f), 0.0f);

        for (int i = 0; i < n_rope_links; i++)
        {
            auto rope_obj = make_shared<Mesh>(
                sphere_mesh_data,
                rope_color);
            auto rope_obj_render = window.add(rope_obj, TransformSim3(
				last_phys_rope_obj->getPos() + Vec3(0,1,0)*rope_link_length,
				Vec3(0.0,0.0,0.0),
				rope_sphere_size
			));
            auto phys_rope_obj = phys.add(rope_obj_render, Verlet::makeSphere(1.0f), 1.0f);

            phys.addConstraint(phys_rope_obj, last_phys_rope_obj, rope_link_length);

            last_phys_rope_obj = phys_rope_obj;
        }
    }



    phys.add(plane_1_render, Verlet::makePlane(Vec3(0,0.7071,-0.7071)), 0.0f);
    phys.add(plane_2_render, Verlet::makePlane(Vec3(0,0,-1)), 0.0f);
    phys.add(plane_3_render, Verlet::makePlane(Vec3(0,-0.7071,-0.7071)), 0.0f);

    float t = 0;
    float dt = 1.0/60.0;
    while (window.render())
    {
        phys.update(dt);

        t += dt;
    }

    return 0;
}