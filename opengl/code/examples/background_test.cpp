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
	Window window(-1, -1, "background test");
    window.setFrameLock(60.0f);
	
	window.setCamFromWorld(makeLookAtTransform(
        Vec3(0,0,5),
        Vec3(-20,0,5),
        Vec3(0,0,1)
    ));
	
	auto back_tex = window.getTextureFromImage(*generateGradientImage(Vec2i(64,64), Vec3(0.05,0.05,0.1), Vec3(0.3,0.3,0.6)));
    auto back = make_shared<Background>(back_tex);
    window.add(back, Transform3(), false, -1);

    auto sprite = make_shared<Sprite>(
		back_tex
	);
    sprite->setDiffuse(Vec3(0,0,0));
    sprite->setAmbient(Vec3(0,0,0));
    sprite->setEmissive(Vec3(1,1,1));
	window.add(sprite, TransformSim3(
		Vec3(200.0,200.0,0.0),
		Vec3(0.0,0.0,0.0),
        100.0
	),true);
	
	Vec3 light_dir(1,0.9,0.8);
	light_dir /= light_dir.norm();
	auto dir_light = make_shared<DirLight>(light_dir, 0.4f);
	window.add(dir_light, Transform3());
	
	window.add(make_shared<Sphere>());
	window.add(make_shared<Sphere>(), Vec3(0,0,5));
	window.add(make_shared<Sphere>(), Vec3(0,0,10));

	while(window.render())
	{
	}
	
	return 0;
}