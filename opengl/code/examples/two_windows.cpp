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
using namespace vephor::ogl;

int main()
{
    Window window_1(800, 800, "window 1", [&](Window* this_window, const Vec2i& window_size){
		Mat4 proj = makePerspectiveProj(45, window_size, 0.1f, 100.0f);
		this_window->setProjectionMatrix(proj);
	});
	window_1.setFrameLock(60.0f);
	Mat4 proj_1 = makePerspectiveProj(45, window_1.getSize(), 0.1f, 100.0f);
	window_1.setProjectionMatrix(proj_1);
	window_1.setCamFromWorld(makeLookAtTransform(
        Vec3(0,0,0),
        Vec3(-12,0,12),
        Vec3(0,0,-1)
    ));
	window_1.add(make_shared<Sphere>(), Transform3());	
	{
		Vec3 light_dir(1,0.9,0.8);
		light_dir /= light_dir.norm();
		auto dir_light = make_shared<DirLight>(light_dir, 0.4f);
		window_1.add(dir_light, Transform3());
	}
	
	Window window_2(800, 800, "window 2", [&](Window* this_window, const Vec2i& window_size){
		Mat4 proj = makePerspectiveProj(45, window_size, 0.1f, 100.0f);
		this_window->setProjectionMatrix(proj);
	});
	window_2.setFrameLock(60.0f);
	Mat4 proj_2 = makePerspectiveProj(45, window_2.getSize(), 0.1f, 100.0f);
    window_2.setProjectionMatrix(proj_2);
	window_2.setCamFromWorld(makeLookAtTransform(
        Vec3(0,0,0),
        Vec3(-12,0,12),
        Vec3(0,0,-1)
    ));
	window_2.add(make_shared<Cylinder>(), Transform3());
	{
		Vec3 light_dir(1,0.9,0.8);
		light_dir /= light_dir.norm();
		auto dir_light = make_shared<DirLight>(light_dir, 0.4f);
		window_2.add(dir_light, Transform3());
	}
	
	while (true)
	{
		bool on_1 = window_1.render();
		bool on_2 = window_2.render();
		
		if (!on_1 && !on_2)
			break;
	}
	
	return 0;
}