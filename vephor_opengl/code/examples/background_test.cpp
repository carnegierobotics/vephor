#include "vephor_ogl.h"

using namespace vephor;

int main()
{
	Window window(-1, -1, "thick_lines");
    window.setFrameLock(60.0f);
	
	window.setCamFromWorld(makeLookAtTransform(
        Vec3(0,0,5),
        Vec3(-20,0,5),
        Vec3(0,0,1)
    ));
	
	auto back_tex = getTextureFromImage(*generateGradientImage(Vec2i(64,64), Vec3(0.05,0.05,0.1), Vec3(0.3,0.3,0.6)));
    auto back = make_shared<Background>(back_tex);
    window.add(back, Transform3(), false, -1);

    auto sprite = make_shared<Sprite>(
		back_tex
	);
    sprite->setDiffuse(Vec3(0,0,0));
    sprite->setAmbient(Vec3(0,0,0));
    sprite->setEmissive(Vec3(1,1,1));
	window.add(sprite, TransformSim3(
		Vec3(200.0,200.0,0.0),
		Vec3(0.0,0.0,0.0),
        100.0
	),true);
	
	Vec3 light_dir(1,0.9,0.8);
	light_dir /= light_dir.norm();
	auto dir_light = make_shared<DirLight>(light_dir, 0.4f);
	window.add(dir_light, Transform3());
	
	window.add(make_shared<Sphere>());
	window.add(make_shared<Sphere>(), Vec3(0,0,5));
	window.add(make_shared<Sphere>(), Vec3(0,0,10));

	while(window.render())
	{
	}
	
	return 0;
}