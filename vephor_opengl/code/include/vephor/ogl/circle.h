#pragma once

#include "window.h"
#include "mesh.h"

namespace vephor
{

class Circle
{
public:
    Circle(float p_rad, float p_thickness, float p_slices = 16)
	{
		MeshData data;
		formCircle(p_rad, p_thickness, p_slices, data);
		mesh = make_shared<Mesh>(data);
	}
	void setColor(const Vec3& p_color)
	{
		mesh->setDiffuse(p_color);
		mesh->setAmbient(p_color);
	}
	void renderOGL(Window* window, const TransformSim3& world_from_body)
	{
		mesh->renderOGL(window, world_from_body);
	}
	void cleanup()
	{
		mesh->cleanup();
	}
private:
	shared_ptr<Mesh> mesh;
};

}