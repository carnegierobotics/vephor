/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

#include <iostream>
#include "vephor_ext.h"
#include <thread>

using namespace vephor;

int main()
{
	Window window_1(640,640,"show 1");
	window_1.setClientMode(true);

	Window window_2(640,640,"show 2");
	
	auto node_1 = window_1.add(make_shared<Sphere>(), Vec3(10,0,0));
	auto node_2 = window_2.add(make_shared<Sphere>(), Vec3(0,10,0));
	
	window_1.add(make_shared<Grid>(10), Transform3());
	window_2.add(make_shared<Grid>(10), Transform3());
	
	float time = 0;
	float dt = 0.015f;
	
	while (true)
	{
		node_1->setPos(Vec3(cos(time),0,sin(time))*10);
		node_2->setPos(Vec3(0,cos(time),sin(time))*10);
		
		bool keep_going = false;
		keep_going |= window_1.render(false);
		keep_going |= window_2.render(false);
		if (!keep_going)
		{
			break;
		}
		
		time += dt;
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(dt*1000)));
	}

	return 0;
}