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
w.add(v4.Cylinder(),(0,0,5))
w.add(v4.Cylinder(),(0,0,-5))

path = []
for i in range(100000):
    t = i/10000.0
    ang = t*2*np.pi
    path.append([np.cos(ang)*3,np.sin(ang)*3,t-5])

l = v4.Lines(path)
l.setColor([1,0,0])
w.add(l,np.zeros(3))
w.render()

w.add(v4.Axes())
w.render()