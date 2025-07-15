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
    void onRemoveFromWindow(Window* window){}
private:
    Window* curr_window = NULL;
    Vec3 strength;
};

class PointLight
{
public:
    PointLight(float strength = 50.0f);
    void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
    {
        if (curr_window != NULL)
            throw std::runtime_error("Point lights can't be added to a window multiple times.");

        curr_window = window;
        light_id = curr_window->addPointLight(node->getPos(), strength);
		node->addTransformCallback([this](const TransformSim3& world_from_node)
		{
			if (curr_window)
				curr_window->updatePointLight(light_id, world_from_node.translation(), strength);
		});
    }
    void renderOGL(Window* window, const TransformSim3& world_from_body){}
    void onRemoveFromWindow(Window* window)
    {
        window->removePointLight(light_id);
        curr_window = NULL;
    }
private:
    Window* curr_window = NULL;
    int light_id;
    float strength;
};

class DirLight
{
public:
    DirLight(const Vec3& p_dir, float p_strength=0.5f):dir(p_dir), strength(p_strength){}
    bool onOverlay() const {return false;}
    void onAddToWindow(Window* window, const shared_ptr<TransformNode>& node)
    {
        curr_window = window;
        curr_window->setDirLight(dir, strength, shadows, shadow_opts);
    }
    void setDir(const Vec3& p_dir)
    {
        dir = p_dir;
        if (curr_window)
            curr_window->setDirLight(dir, strength, shadows, shadow_opts);
    }
    void enableShadows(const ShadowOptions& opts=ShadowOptions())
    {
        shadows = true;
        shadow_opts = opts;
        if (curr_window)
            curr_window->setDirLight(dir, strength, shadows, shadow_opts);
    }
	void renderOGL(Window* window, const TransformSim3& world_from_body){}
    void onRemoveFromWindow(Window* window){}
private:
    Window* curr_window = NULL;
    Vec3 dir;
    float strength;
    bool shadows = false;
    ShadowOptions shadow_opts;
};

}