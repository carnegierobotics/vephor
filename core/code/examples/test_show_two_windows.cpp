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
		keep_going |= window_1.render();
		keep_going |= window_2.render();
		if (!keep_going)
		{
			break;
		}
		
		time += dt;
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(dt*1000)));
	}

	return 0;
}