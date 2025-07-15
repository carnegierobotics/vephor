/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#include "vephor.h"
#include "vephor_ogl.h"

using namespace vephor;
using namespace vephor::ogl;

int main()
{
    Window window;

    window.setFrameLock(60.0f);

    while (window.render())
    {
    }

    return 0;
}