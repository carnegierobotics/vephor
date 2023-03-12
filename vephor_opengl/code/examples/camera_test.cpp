#include "vephor_ogl.h"

using namespace vephor;

int main()
{
    Window window_1(800, 800, "window 1", [&](Window* this_window, const Vec2i& window_size){
		v4print "Window 1 resize:", window_size.transpose();
		Mat4 proj = makePerspectiveProj(45, window_size, 0.1f, 100.0f);
		this_window->setProjectionMatrix(proj);
	});
	window_1.setFrameLock(60.0f);
	Mat4 proj_1 = makePerspectiveProj(45, window_1.getSize(), 0.1f, 100.0f);
	window_1.setProjectionMatrix(proj_1);
	window_1.add(make_shared<Axes>(), Vec3(0,0,-20))->setScale(1.0f);
	window_1.add(make_shared<Cone>(), Vec3(0,0,-25))->setScale(2.0f).setOrient(Vec3(M_PI/2.0f,0,0));	
	window_1.add(make_shared<Sphere>(), Vec3(0,0,-30))->setScale(3.0f);	
	window_1.add(make_shared<Cylinder>(), Vec3(0,0,-35))->setScale(4.0f).setOrient(Vec3(M_PI/2.0f,0,0));
	{
		Vec3 light_dir(1,0.9,0.8);
		light_dir /= light_dir.norm();
		auto dir_light = make_shared<DirLight>(light_dir, 0.4f);
		window_1.add(dir_light, Transform3());
	}
	
	Window window_2(800, 800, "window 2", [&](Window* this_window, const Vec2i& window_size){
		v4print "Window 2 resize:", window_size.transpose();
		Mat4 proj = makeOrthoProj(Vec3(-10,-10,0), Vec3(10,10,100));
		this_window->setProjectionMatrix(proj);
	});
	window_2.setFrameLock(60.0f);
	Mat4 proj_2 = makeOrthoProj(Vec3(-10,-10,0), Vec3(10,10,100));
    window_2.setProjectionMatrix(proj_2);
	window_2.add(make_shared<Axes>(), Vec3(0,0,-20))->setScale(1.0f);
	window_2.add(make_shared<Cone>(), Vec3(0,0,-25))->setScale(2.0f).setOrient(Vec3(M_PI/2.0f,0,0));
	window_2.add(make_shared<Sphere>(), Vec3(0,0,-30))->setScale(3.0f);
	window_2.add(make_shared<Cylinder>(), Vec3(0,0,-35))->setScale(4.0f).setOrient(Vec3(M_PI/2.0f,0,0));
	{
		Vec3 light_dir(1,0.9,0.8);
		light_dir /= light_dir.norm();
		auto dir_light = make_shared<DirLight>(light_dir, 0.4f);
		window_2.add(dir_light, Transform3());
	}
	
	while (true)
	{
		bool on_1 = window_1.render();
		bool on_2 = window_2.render();
		
		if (!on_1 && !on_2)
			break;
	}
}