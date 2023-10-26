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

#include "vephor.h"
#include "vephor_ogl.h"
#include "vephor/verlet.h"
#include <random>
#include <chrono>

using namespace vephor;

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
    
    auto plane_4 = make_shared<Lines>(
        plane_verts,
		MatX(),
        Vec3(0.0,0.0,1.0));
    auto plane_4_render = window.add(plane_4, Transform3(
		Vec3(0.0,0.0,-20.0),
		Vec3(0.0,0.0,0.0)
	));

    auto sphere_1 = make_shared<Lines>(
        sphere_verts,
		MatX(),
        Vec3(0.0,0.0,1.0));
    auto sphere_1_render = window.add(sphere_1, TransformSim3(
		Vec3(0.0,0.0,-50.0),
		Vec3(0.0,0.0,0.0),
		15.0f
	));

    auto sphere_2 = make_shared<Lines>(
        sphere_verts,
		MatX(),
        Vec3(1.0,1.0,1.0));
    auto sphere_2_render = window.add(sphere_2, TransformSim3(	
		Vec3(0.0,30.0,-60.0),
		Vec3(0.0,0.0,0.0),
		10.0f
	));

    auto sphere_3 = make_shared<Lines>(
        sphere_verts,
		MatX(),
        Vec3(1.0,1.0,1.0));
    auto sphere_3_render = window.add(sphere_3, TransformSim3(
		Vec3(0.0,-30.0,-60.0),
		Vec3(0.0,0.0,0.0),
		10.0f
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
        Vec3(-10,0,-40.0f),
        Vec3(0,0,-1)
    ));

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(-16.0,16.0);

    // Small spheres
    for (size_t i = 0; i < 200; i++)
    {
        auto sphere_obj = make_shared<Mesh>(
            sphere_mesh_data,
            Vec3(0.0,1.0,1.0));
        auto sphere_obj_render = window.add(sphere_obj, Transform3(
			Vec3(0.0,distribution(generator),-40.0+distribution(generator)),
			Vec3(0.0,0.0,0.0)
		));

        phys.add(sphere_obj_render, Verlet::makeSphere(1.0f), 1.0f);
    }

    // Large spheres
    for (size_t i = 0; i < 30; i++)
    {
        auto sphere_obj = make_shared<Mesh>(
            sphere_mesh_data,
            Vec3(0.25,0.5,0.5));
        auto sphere_obj_render = window.add(sphere_obj, TransformSim3(
			Vec3(0.0,distribution(generator),-40.0+distribution(generator)),
			Vec3(0.0,0.0,0.0),
			2.0f
		));

        phys.add(sphere_obj_render, Verlet::makeSphere(1.0f), 5.0f);
    }

    // Very large spheres
    for (size_t i = 0; i < 10; i++)
    {
        auto sphere_obj = make_shared<Mesh>(
            sphere_mesh_data,
            Vec3(0.25,0.25,0.25));
        auto sphere_obj_render = window.add(sphere_obj,
			TransformSim3(
                Vec3(0.0,distribution(generator),-40.0+distribution(generator)),
                Vec3(0.0,0.0,0.0),
				4.0f
            )
		);

        phys.add(sphere_obj_render, Verlet::makeSphere(1.0f), 25.0f);
    }

    phys.add(plane_1_render, Verlet::makePlane(Vec3(0,0.7071,-0.7071)), 0.0f);
    phys.add(plane_2_render, Verlet::makePlane(Vec3(0,0,-1)), 0.0f);
    phys.add(plane_3_render, Verlet::makePlane(Vec3(0,-0.7071,-0.7071)), 0.0f);
    phys.add(plane_4_render, Verlet::makePlane(Vec3(0,0,-1)), 0.0f, true);
    phys.add(sphere_1_render, Verlet::makeSphere(1.0f), 0.0f, true);
    phys.add(sphere_2_render, Verlet::makeSphere(1.0f), 0.0f);
    phys.add(sphere_3_render, Verlet::makeSphere(1.0f), 0.0f);

    float t = 0;
    float dt = 1.0/60.0;
    while (window.render())
    {
        auto start_time = std::chrono::high_resolution_clock::now();
        phys.update(dt);
        auto end_time = std::chrono::high_resolution_clock::now();

        float elapsed_time_ms = std::chrono::duration<float, std::milli>(end_time-start_time).count();

        v4print "Physics time (ms):", elapsed_time_ms;

        t += dt;
    }

    return 0;
}