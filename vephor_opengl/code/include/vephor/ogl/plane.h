#pragma once

#include "window.h"
#include "mesh.h"

namespace vephor
{

class Plane
{
public:
    Plane(const Vec2& p_size)
	{
		MeshData data;
		formPlane(p_size, data);
		mesh = make_shared<Mesh>(data);
	}
	void setColor(const Color& p_color)
	{
		mesh->setDiffuse(p_color.getRGB());
		mesh->setAmbient(p_color.getRGB());
		mesh->setOpacity(p_color.getAlpha());
	}
	void setTexture(const shared_ptr<Texture>& p_tex){mesh->setTexture(p_tex);}
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