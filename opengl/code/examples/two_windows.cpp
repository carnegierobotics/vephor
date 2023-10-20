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

#include "vephor_ogl.h"

using namespace vephor;

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