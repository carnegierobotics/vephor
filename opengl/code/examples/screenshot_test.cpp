#include "vephor_ogl.h"

using namespace vephor;

int main()
{
	Window window(-1, -1, "screenshot test");
    window.setFrameLock(60.0f);
	
	window.setCamFromWorld(makeLookAtTransform(
        Vec3(0,0,5),
        Vec3(-20,0,5),
        Vec3(0,0,1)
    ));

    auto back_tex = window.getTextureFromImage(*generateGradientImage(Vec2i(64,64), Vec3(0.05,0.05,0.1), Vec3(0.3,0.3,0.6)));
    auto back = make_shared<Background>(back_tex);
    window.add(back, Transform3(), false, -1);
	
	Vec3 light_dir(1,0.9,-0.8);
	light_dir /= light_dir.norm();
	auto dir_light = make_shared<DirLight>(light_dir, 0.4f);
	window.add(dir_light, Transform3());
	
	window.add(make_shared<Sphere>());
	window.add(make_shared<Sphere>(), Vec3(0,0,5));
	window.add(make_shared<Sphere>(), Vec3(0,0,10));
    window.add(make_shared<Plane>(Vec2(10,10)), Vec3(0,0,-2));

	window.render();
    window.render();

    v4print "Saving screenshot...";
    auto screenshot = window.getScreenImage();
    saveImage(getTempDir()+"/screenshot.png", screenshot);

    v4print "Saving depth screenshot...";
    auto depth_screenshot = window.getDepthImage();
    saveImage(getTempDir()+"/depth_screenshot.png", depth_screenshot);

    v4print "Exiting...";
	
	return 0;
}