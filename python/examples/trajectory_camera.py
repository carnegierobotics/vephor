#! /usr/bin/env python3

#
# Copyright 2024
# Carnegie Robotics, LLC
# 4501 Hatfield Street, Pittsburgh, PA 15201
# https://www.carnegierobotics.com
#
# All rights reserved.
#
# Significant history (date, user, job code, action):
#   2024-11-27, emusser@carnegierobotics.com, 2045.01, Created file.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Carnegie Robotics, LLC nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL CARNEGIE ROBOTICS, LLC BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
