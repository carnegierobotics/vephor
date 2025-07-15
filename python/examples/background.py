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

import vephor as v4

w = v4.Window()

w.setSpinCameraMode(from_dist=20,from_angle_deg=-30,s_per_rev=20)
w.setCheckerBackground([0.18,0.18,0.18],[0.22,0.22,0.22])

w.add(v4.Grid(10))

w.render()