#pragma once

#include "window.h"
#include "mesh.h"

namespace vephor
{

class Cylinder
{
public:
    Cylinder(float p_rad = 1.0f, float p_height = 1.0f, int p_slices = 16)
	{
		MeshData data;
		formCylinder(p_rad, p_height, p_slices, data);
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