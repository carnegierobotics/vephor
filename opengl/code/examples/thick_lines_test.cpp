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
	Window window(-1, -1, "thick_lines");
    window.setFrameLock(60.0f);
	
	window.setCamFromWorld(makeLookAtTransform(
        Vec3(0,0,5),
        Vec3(-20,0,5),
        Vec3(0,0,1)
    ));
	
	MatX verts(100, 3);
	MatX colors(100, 4);
	
	const float spiral_rad = 1.5f;
	
	for (int i = 0; i < 100; i++)
	{
		float angle = (float)i / 25.0f * 2.0f * M_PI;
		verts.row(i) = Vec3(cos(angle)*spiral_rad,sin(angle)*spiral_rad,i*0.1);
		
		float color_ang = i/10.0f*2*M_PI;
		Vec2 color_vec(cos(color_ang), sin(color_ang));
		
		float red_ang = 0*2*M_PI;
		float green_ang = 1.0f/3.0f*2*M_PI;
		float blue_ang = 2.0f/3.0f*2*M_PI;
		
		Vec2 red_vec(cos(red_ang), sin(red_ang));
		Vec2 green_vec(cos(green_ang), sin(green_ang));
		Vec2 blue_vec(cos(blue_ang), sin(blue_ang));
		
		colors.row(i) = Vec4(
			clamp(color_vec.dot(red_vec)*4,0,1),
			clamp(color_vec.dot(green_vec)*4,0,1),
			clamp(color_vec.dot(blue_vec)*4,0,1),
			1);
	}
	
	Vec3 light_dir(1,0.9,0.8);
	light_dir /= light_dir.norm();
	auto dir_light = make_shared<DirLight>(light_dir, 0.4f);
	window.add(dir_light, Transform3());
	
	window.add(make_shared<Sphere>());
	window.add(make_shared<Sphere>(), Vec3(0,0,5));
	window.add(make_shared<Sphere>(), Vec3(0,0,10));
	
	auto tl = make_shared<ThickLines>(verts, colors);
	tl->setLineWidth(0.025f);
	window.add(tl);

	float t = 0;
	float dt = 1.0f / 60.0f;
	while(window.render())
	{
		float mag = -13 - 7*cos(t);
		float ang = t / 5.0 * 2 * M_PI;
		
		window.setCamFromWorld(makeLookAtTransform(
			Vec3(0,0,5),
			Vec3(0,0,5)+Vec3(cos(ang)*mag,0,sin(ang)*mag),
			Vec3(0,1,0)
		));
		
		t += dt;
	}
	
	return 0;
}