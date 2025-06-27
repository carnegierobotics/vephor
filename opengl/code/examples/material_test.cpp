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
    Window window(-1, -1, "basic");

    window.setFrameLock(60.0f);

    MaterialBuilder builder;
    builder.tex = true;
    builder.normal_map = true;
    builder.dir_light = true;
    builder.point_lights = true;
    v4print builder.produceVertexShader();
    v4print builder.produceFragmentShader();
	
    /*while (window.render())
    {
    }*/

    return 0;
}