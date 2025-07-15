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
import numpy as np

w = v4.Window()
w.add(v4.Sphere(),(5,0,0))
w.add(v4.Sphere(),(-5,0,0))
w.add(v4.Sphere(),(0,5,0))
w.add(v4.Sphere(),(0,-5,0))

w.add(v4.Axes())

w.add(v4.Text("Test text"), (0,0,0), scale=100, overlay=True)

w.render()