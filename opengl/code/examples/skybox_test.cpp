/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

#include "vephor.h"
#include "vephor_ogl.h"

using namespace vephor;

int main()
{
    AssetManager asset_mgr;
    TrackballCamera cam_mgr;

    Window window(-1, -1, "basic");

    float fps = 60.0f;
	window.setFrameLock(fps);
	float dt = 1.0f / fps;

    cam_mgr.setup({
		{"to", {0,0,0}},
		{"from", {-50,0,0}},
		{"up", {0,0,-1}},
        {"3d", false}
	}, window, asset_mgr);

	BoundManager bound_mgr(&window);
	ControlInfo control_info;
    control_info.setStandardWindowCallbacks(window);


    string base_asset_dir = getBaseAssetDir()+"/assets";
    string skybox_dir = base_asset_dir+"/Nalovardo";

    auto cube_tex = window.getCubeTextureFromDir(skybox_dir);

    auto skybox = make_shared<Skybox>(cube_tex);
    window.add(skybox);


    for (int i = -250; i <= 250; i += 25)
    for (int j = -250; j <= 250; j += 25) 
    {
        auto mesh = make_shared<Sphere>();
        float i_peru = (i + 250) / 500.0f;
        float j_peru = (j + 250) / 500.0f;
        mesh->setColor(Vec3(i_peru,0,j_peru));
        auto mesh_render = window.add(mesh, TransformSim3(
            Vec3(i,j,-2.0),
            Vec3(0.0,0.0,0.0),
            0.5f
        ));
    }



    auto grid = make_shared<Grid>(250);
    window.add(grid);
    bound_mgr.addBoundSphere(10.0f, Transform3(Vec3(0,0,0)));


    Vec3 light_dir(-1,-1,1);
    light_dir /= light_dir.norm();
    auto dir_light = make_shared<DirLight>(light_dir, 0.4f);
    window.add(dir_light, Transform3());

    auto ambient_light = make_shared<AmbientLight>(Vec3(0.6f,0.6f,0.6f));
    window.add(ambient_light);

    Mat4 proj = makePerspectiveProj(45, window.getSize(), 0.1f, 100.0f);
    window.setProjectionMatrix(proj);

    cam_mgr.autoFitPoints(window, bound_mgr.calcCameraBoundPoints());

    float t = 0;
    while (window.render())
    {
        cam_mgr.update(window, dt, control_info);
		control_info.onUpdate();

        t += dt;
    }

    return 0;
}