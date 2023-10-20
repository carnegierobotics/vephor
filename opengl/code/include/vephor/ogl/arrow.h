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
#include "mesh.h"

namespace vephor
{

class Arrow
{
public:
    Arrow(float p_length = 1.0f, float p_rad = 1.0f, int p_slices = 16)
	{
		float arrow_length = p_rad;
		float base_length = p_length - arrow_length;
		
		{
			auto data = formCone(p_rad, arrow_length, p_slices);
			head = make_shared<Mesh>(data);
			body_from_arrow_head = Transform3(Vec3(0,0,base_length));
		}
		
		{
			auto data = formCylinder(p_rad*0.25f, base_length, p_slices);
			body = make_shared<Mesh>(data);
			body_from_arrow_body = Transform3(Vec3(0,0,base_length/2));
		}
	}
	Arrow(const Vec3& start, const Vec3& end, float p_rad = 1.0f, int p_slices = 16)
	{
		float arrow_length = p_rad;
		float base_length = (end - start).norm() - arrow_length;
		
		{
			auto data = formCone(p_rad, arrow_length, p_slices);
			head = make_shared<Mesh>(data);
			body_from_arrow_head = Transform3(Vec3(0,0,base_length));
		}
		
		{
			auto data = formCylinder(p_rad*0.25f, base_length, p_slices);
			body = make_shared<Mesh>(data);
			body_from_arrow_body = Transform3(Vec3(0,0,base_length/2));
		}
		
		Transform3 new_body_from_body = Transform3(start, makeFacing(end - start));
		body_from_arrow_head = new_body_from_body * body_from_arrow_head;
		body_from_arrow_body = new_body_from_body * body_from_arrow_body;
	}
	void setColor(const Vec3& p_color)
	{
		head->setDiffuse(p_color);
		head->setAmbient(p_color);
		body->setDiffuse(p_color);
		body->setAmbient(p_color);
	}
	void renderOGL(Window* window, const TransformSim3& world_from_body)
	{
		TransformSim3 world_from_arrow_head = world_from_body * body_from_arrow_head;
		TransformSim3 world_from_arrow_body = world_from_body * body_from_arrow_body;
		
		head->renderOGL(window, world_from_arrow_head);
		body->renderOGL(window, world_from_arrow_body);
	}
	void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
    {
		head->onAddToWindow(window, node);
		body->onAddToWindow(window, node);
	}
	void onRemoveFromWindow(Window* window)
	{
		head->onRemoveFromWindow(window);
		body->onRemoveFromWindow(window);
	}
private:
	Transform3 body_from_arrow_head;
	Transform3 body_from_arrow_body;
	
	shared_ptr<Mesh> head;
	shared_ptr<Mesh> body;
};

}