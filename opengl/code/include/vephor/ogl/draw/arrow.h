/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#pragma once

#include "../window.h"
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
	void setColor(const Color& p_color)
	{
		head->setDiffuse(p_color);
		head->setAmbient(p_color);
		head->setOpacity(p_color.getAlpha());
		body->setDiffuse(p_color);
		body->setAmbient(p_color);
		body->setOpacity(p_color.getAlpha());
	}
	void setColor(const Vec4& p_color)
	{
        setColor(Color(p_color));
	}
	void setColor(const Vec3& p_color)
	{
        setColor(Color(p_color));
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