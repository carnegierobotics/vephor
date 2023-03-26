#pragma once

#include "window.h"

namespace vephor
{
	
class AmbientLight
{
public:
    AmbientLight(const Vec3& p_strength):strength(p_strength){}
    bool onOverlay() const {return false;}
    void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
    {
        curr_window = window;
        curr_window->setAmbientLight(strength);
    }
    void setStrength(const Vec3& p_strength)
    {
        strength = p_strength;
        curr_window->setAmbientLight(strength);
    }
    void renderOGL(Window* window, const TransformSim3& world_from_body){}
    void cleanup(){}
private:
    Window* curr_window = NULL;
    Vec3 strength;
};


template <>
void onAddToWindow<AmbientLight>(AmbientLight* object, Window* window, const shared_ptr<TransformNode>& node);

class PointLight
{
public:
    PointLight(float strength = 50.0f);
    void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
    {
        curr_window = window;
        light_id = curr_window->addPointLight(node->getPos(), strength);
		node->addTransformCallback([this](const TransformSim3& world_from_node)
		{
			if (curr_window)
				curr_window->updatePointLight(light_id, world_from_node.translation(), strength);
		});
    }
    void renderOGL(Window* window, const TransformSim3& world_from_body){}
    void cleanup(){}
private:
    Window* curr_window = NULL;
    int light_id;
    float strength;
};

template <>
void onAddToWindow<PointLight>(PointLight* object, Window* window, const shared_ptr<TransformNode>& node);

class DirLight
{
public:
    DirLight(const Vec3& p_dir, float p_strength=0.5f):dir(p_dir), strength(p_strength){}
    bool onOverlay() const {return false;}
    void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
    {
        curr_window = window;
        curr_window->setDirLight(dir, strength);
    }
    void setDir(const Vec3& p_dir)
    {
        dir = p_dir;
        curr_window->setDirLight(dir, strength);
    }
	void renderOGL(Window* window, const TransformSim3& world_from_body){}
    void cleanup(){}
private:
    Window* curr_window = NULL;
    Vec3 dir;
    float strength;
};

template <>
void onAddToWindow<DirLight>(DirLight* object, Window* window, const shared_ptr<TransformNode>& node);

}