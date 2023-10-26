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

#pragma once

#include "window.h"
#include "cylinder.h"

namespace vephor
{

class Axes
{
public:
    Axes(float p_size = 1.0f)
	: size(p_size)
	{
		axes[0] = make_shared<Cylinder>(0.1f, p_size, 6);
		axes[0]->setColor(Vec3(1,0,0));
		
		axes[1] = make_shared<Cylinder>(0.1f, p_size, 6);
		axes[1]->setColor(Vec3(0,1,0));
		
		axes[2] = make_shared<Cylinder>(0.1f, p_size, 6);
		axes[2]->setColor(Vec3(0,0,1));
	}
	void renderOGL(Window* window, const TransformSim3& world_from_body)
	{
		TransformSim3 axis_0(Vec3(size*0.5f,0,0), makeFacing(Vec3(1,0,0)));
		TransformSim3 axis_1(Vec3(0,size*0.5f,0), makeFacing(Vec3(0,1,0)));
		TransformSim3 axis_2(Vec3(0,0,size*0.5f), makeFacing(Vec3(0,0,1)));
		axis_0 = world_from_body * axis_0;
		axis_1 = world_from_body * axis_1;
		axis_2 = world_from_body * axis_2;
		
		axes[0]->renderOGL(window, axis_0);
		axes[1]->renderOGL(window, axis_1);
		axes[2]->renderOGL(window, axis_2);
	}
	void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
    {
		axes[0]->onAddToWindow(window, node);
		axes[1]->onAddToWindow(window, node);
		axes[2]->onAddToWindow(window, node);
	}
	void onRemoveFromWindow(Window* window)
	{
		axes[0]->onRemoveFromWindow(window);
		axes[1]->onRemoveFromWindow(window);
		axes[2]->onRemoveFromWindow(window);
	}
private:
	array<shared_ptr<Cylinder>, 3> axes;
	float size;
};

}