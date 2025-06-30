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
		{"up", {0,0,-1}}
	}, window, asset_mgr);

	BoundManager bound_mgr(&window);
	ControlInfo control_info;
    control_info.setStandardWindowCallbacks(window);


    auto mesh = make_shared<Sphere>();
    
    auto mesh_render = window.add(mesh, TransformSim3(
		Vec3(0.0,0.0,0.0),
		Vec3(0.0,0.0,0.0),
		1.0f
	));
    bound_mgr.addBoundSphere(1.0f, Transform3(Vec3(0,0,0)));


    Vec3 light_dir(-1,-1,1);
    light_dir /= light_dir.norm();
    auto dir_light = make_shared<DirLight>(light_dir, 0.4f);
    window.add(dir_light, Transform3());

    Mat4 proj = makePerspectiveProj(45, window.getSize(), 0.1f, 100.0f);
    window.setProjectionMatrix(proj);

    cam_mgr.autoFitPoints(window, bound_mgr.calcCameraBoundPoints());

    float t = 0;
    while (window.render())
    {
        cam_mgr.update(window, dt, control_info);
		control_info.onUpdate();

        t += dt;
    }

    return 0;
}