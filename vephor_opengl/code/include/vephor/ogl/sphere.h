#pragma once

#include "window.h"
#include "mesh.h"

namespace vephor
{

class Sphere
{
public:
    Sphere(float p_rad = 1.0f, int p_slices = 16, int p_stacks = 16)
	{
		MeshData data;
		formSphere(p_slices, p_stacks, data);
		data.verts *= p_rad;
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
private:
	shared_ptr<Mesh> mesh;
};

}