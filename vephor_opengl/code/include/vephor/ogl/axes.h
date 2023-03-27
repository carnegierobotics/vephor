#pragma once

#include "window.h"
#include "cylinder.h"

namespace vephor
{

class Axes
{
public:
    Axes(float p_size = 1.0f)
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
		TransformSim3 axis_0(Vec3(0.5f,0,0), makeFacing(Vec3(1,0,0)));
		TransformSim3 axis_1(Vec3(0,0.5f,0), makeFacing(Vec3(0,1,0)));
		TransformSim3 axis_2(Vec3(0,0,0.5f), makeFacing(Vec3(0,0,1)));
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
};

}