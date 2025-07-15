/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#include "vephor_ext.h"

#include <vector>

using namespace vephor;

int main()
{
    Window window(/* width */ 1920.0F, /* height */ 1080.0F);

    //
    // Constants
    //

    const Vec3 origin{500.0F, -1000.0F, 2000.0F};
    const float offset{5.0F};

    const Vec3 &axes_position = origin;
    const Vec3 sphere_position = origin - Vec3{offset, 0.0F, 0.0F};
    const Vec3 cone_position = origin + Vec3{offset, 0.0F, 0.0F};
    const Vec3 cylinder_position = origin - Vec3{0.0F, offset, 0.0F};
    const Vec3 cube_position = origin + Vec3{0.0F, offset, 0.0F};

    //
    // Add several render objects
    //

    const auto axes = make_shared<Axes>();
    window.add(axes, axes_position);

    auto sphere = make_shared<Sphere>();
    sphere->setColor({255.0, 0.0, 0.0});
    window.add(sphere, sphere_position);

    auto cone = make_shared<Cone>();
    cone->setColor({255.0, 255.0, 0.0});
    window.add(cone, cone_position);

    auto cylinder = make_shared<Cylinder>();
    cylinder->setColor({0.0, 255.0, 0.0});
    window.add(cylinder, cylinder_position);

    auto cube = make_shared<Cube>();
    cube->setColor({0.0, 0.0, 255.0});
    window.add(cube, cube_position);

    //
    // Configure the camera trajectory
    //

    const Vec3 up{0.0F, 0.0F, -1.0F};
    const std::vector<Window::CameraTrajectoryNode> trajectory{
        {.time = 0,
         .to = origin,
         .from = origin + Vec3{-10.0F * offset, -10.0F * offset, 0.0F},
         .up_hint = {1.0F, 1.0F, 0.0F}},
        {.time = 4,
         .to = origin,
         .from = origin + Vec3{-4.0F * offset, 0.0F, -2.0F * offset},
         .up_hint = {0.0F, 0.0F, -1.0F}},
        {.time = 8,
         .to = sphere_position,
         .from = origin + Vec3{0.0F, -2.0F * offset, -0.5F * offset},
         .up_hint = {0.0F, -1.0F, 0.0F}},
        {.time = 12,
         .to = sphere_position,
         .from = origin + Vec3{-2.0F * offset, -2.0F * offset, 0.0F},
         .up_hint = {0.0F, 0.0F, 1.0F}},
        {.time = 16,
         .to = cylinder_position,
         .from = origin + Vec3{-2.0F * offset, -2.0F * offset, 0.0F},
         .up_hint = {0.0F, 0.0F, 1.0F}},
        {.time = 20,
         .to = cylinder_position,
         .from = origin + Vec3{-offset, -offset, 0.0F},
         .up_hint = {1.0F, 0.0F, 1.0F}},
        {.time = 24,
         .to = origin,
         .from = origin + Vec3{-4.0F * offset, -4.0F * offset, 0.0F},
         .up_hint = {0.0F, 0.0F, 1.0F}}};

    window.setTrajectoryCameraMode(/* trajectory */ trajectory,
                                   /* motion_mode */ Window::TrajectoryCameraMotionMode::OSCILLATE,
                                   /* speed */ 1.0F,
                                   /* start_time */ 0.0F,
                                   /* polynomial_degree */ 3);

    //
    // Render
    //

    window.render();

    return 0;
}
