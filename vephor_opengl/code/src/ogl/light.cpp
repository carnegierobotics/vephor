#include "vephor/ogl/light.h"

namespace vephor
{

template <>
void onAddToWindow<AmbientLight>(AmbientLight* object, Window* window, const shared_ptr<TransformNode>& node)
{
	object->onAddToWindow(window, node);
}

PointLight::PointLight(float p_strength)
: strength(p_strength)
{    
}

template <>
void onAddToWindow<PointLight>(PointLight* object, Window* window, const shared_ptr<TransformNode>& node)
{
	object->onAddToWindow(window, node);
}

template <>
void onAddToWindow<DirLight>(DirLight* object, Window* window, const shared_ptr<TransformNode>& node)
{
	object->onAddToWindow(window, node);
}

}