/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#include "vephor_ogl.h"

using namespace vephor;

int main()
{
	AssetManager asset_mgr;
    TrackballCamera cam_mgr;

    Window window(800, 800, "window 1", [&](Window* this_window, const Vec2i& window_size){
        cam_mgr.resizeWindow(*this_window);
	});

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
	window.setLeftMouseButtonPressCallback([&](){
		control_info.onLeftMouseButtonPress(window);
	});
	window.setLeftMouseButtonReleaseCallback([&](){
		control_info.onLeftMouseButtonRelease();
	});
	window.setRightMouseButtonPressCallback([&](){
		control_info.onRightMouseButtonPress(window);
    });
	window.setRightMouseButtonReleaseCallback([&](){
		control_info.onRightMouseButtonRelease();
	});
	window.setKeyPressCallback([&](int key){
		control_info.onKeyPress(key);
	});
	window.setKeyReleaseCallback([&](int key){
		control_info.onKeyRelease(key);
	});
	window.setScrollCallback([&](float amount){
		control_info.onScroll(amount);
	});


	window.add(make_shared<Sphere>(), Transform3(Vec3(0,5,0)));	
	bound_mgr.addBoundSphere(1.0f, Transform3(Vec3(0,5,0)));

	window.add(make_shared<Sphere>(), Transform3(Vec3(0,-5,0)));	
	bound_mgr.addBoundSphere(1.0f, Transform3(Vec3(0,-5,0)));

	window.add(make_shared<Sphere>(), Transform3(Vec3(0,0,5)));	
	bound_mgr.addBoundSphere(1.0f, Transform3(Vec3(0,0,5)));

	window.add(make_shared<Sphere>(), Transform3(Vec3(0,0,-5)));	
	bound_mgr.addBoundSphere(1.0f, Transform3(Vec3(0,0,-5)));

	{
		Vec3 light_dir(1,0.9,0.8);
		light_dir /= light_dir.norm();
		auto dir_light = make_shared<DirLight>(light_dir, 0.4f);
		window.add(dir_light, Transform3());
	}

	cam_mgr.autoFitPoints(window, bound_mgr.calcCameraBoundPoints());
	
	while (window.render())
	{
		cam_mgr.update(window, dt, control_info);
		control_info.onUpdate();
	}
	
	return 0;
}