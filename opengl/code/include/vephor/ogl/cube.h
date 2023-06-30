#pragma once

#include "window.h"
#include "mesh.h"

namespace vephor
{

class Cube
{
public:
    Cube(float p_rad = 1.0f)
	{
		auto data = formCube();
		data.verts *= p_rad;
		mesh = make_shared<Mesh>(data);
	}
	void setColor(const Color& p_color)
	{
		mesh->setDiffuse(p_color);
		mesh->setAmbient(p_color);
	}
	void renderOGL(Window* window, const TransformSim3& world_from_body)
	{
		mesh->renderOGL(window, world_from_body);
	}
	void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
    {
		mesh->onAddToWindow(window, node);
	}
	void onRemoveFromWindow(Window* window)
	{
		mesh->onRemoveFromWindow(window);
	}
private:
	shared_ptr<Mesh> mesh;
};

}