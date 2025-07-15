#! /usr/bin/env python3

#
# Copyright 2025
# Carnegie Robotics, LLC
# 4501 Hatfield Street, Pittsburgh, PA 15201
# https://www.carnegierobotics.com
#
# This source code is licensed under the Apache License, Version 2.0
# found in the LICENSE file in the root directory of this source tree.
#


"""
Miniature test for Python bindings of the vephor.Window.setTrajectoryCameraMode() method.

Duplicates the behavior in core/code/examples/mini_test_show_trajectory_camera.cpp.
"""


import numpy as np
import vephor as v4


if __name__ == "__main__":
    w = v4.Window(width=1920, height=1080)

    #
    # Constants
    #

    origin = np.array([500.0, -1000.0, 2000.0])
    offset = 5.0

    axes_position = origin
    sphere_position = origin - np.array([offset, 0, 0])
    cone_position = origin + np.array([offset, 0, 0])
    cylinder_position = origin - np.array([0, offset, 0])
    cube_position = origin + np.array([0, offset, 0])

    #
    # Add several render objects
    #

    axes = v4.Axes()
    w.add(axes, axes_position)

    sphere = v4.Sphere()
    sphere.setColor([255, 0, 0])
    w.add(sphere, sphere_position)

    cone = v4.Cone()
    cone.setColor([255, 255, 0])
    w.add(cone, cone_position)

    cylinder = v4.Cylinder()
    cylinder.setColor([0, 255, 0])
    w.add(cylinder, cylinder_position)

    cube = v4.Cube()
    cube.setColor([0, 0, 255])
    w.add(cube, cube_position)

    #
    # Configure the camera trajectory
    #

    trajectory = [
        v4.Window.CameraTrajectoryNode(
            time=0.0,
            to_point=origin,
            from_point=origin + np.array([-10 * offset, -10 * offset, 0]),
            up_hint=[1.0, 1.0, 0.0],
        ),
        v4.Window.CameraTrajectoryNode(
            time=4.0,
            to_point=origin,
            from_point=origin + np.array([-4 * offset, 0, -2 * offset]),
            up_hint=[0.0, 0.0, -1.0],
        ),
        v4.Window.CameraTrajectoryNode(
            time=8.0,
            to_point=sphere_position,
            from_point=origin + np.array([0, -2 * offset, -0.5 * offset]),
            up_hint=[0.0, -1.0, 0.0],
        ),
        v4.Window.CameraTrajectoryNode(
            time=12.0,
            to_point=sphere_position,
            from_point=origin + np.array([-2 * offset, -2 * offset, 0]),
            up_hint=[0.0, 0.0, 1.0],
        ),
        v4.Window.CameraTrajectoryNode(
            time=16.0,
            to_point=cylinder_position,
            from_point=origin + np.array([-2 * offset, -2 * offset, 0]),
            up_hint=[0.0, 0.0, 1.0],
        ),
        v4.Window.CameraTrajectoryNode(
            time=20.0,
            to_point=cylinder_position,
            from_point=origin + np.array([-offset, -offset, 0]),
            up_hint=[1.0, 0.0, 1.0],
        ),
        v4.Window.CameraTrajectoryNode(
            time=24.0,
            to_point=origin,
            from_point=origin + np.array([-4 * offset, 4 * offset, offset]),
            up_hint=[0.0, 0.0, 1.0],
        ),
    ]

    w.setTrajectoryCameraMode(
        trajectory=trajectory,
        motion_mode=v4.Window.TrajectoryCameraMotionMode.OSCILLATE,
        speed=3.0,
        start_time=0.0,
        polynomial_degree=3,
    )

    #
    # Render
    #

    w.render()
