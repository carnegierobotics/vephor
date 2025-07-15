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

import os

import vephor as v4
import numpy as np
import time

v4.Window.setClientMode()

w=v4.Window()

node = w.add(np.zeros(3))
w.add(v4.Axes(), np.zeros(3))
w.add(v4.Grid(5), np.zeros(3))
for i in range(16):
    ang = 2*np.pi/16*i
    w.add(v4.Sphere(0.25), (np.cos(ang)*5,np.sin(ang)*5,0)).setParent(node)

t = 0
dt = 0.015
while 1:
    w.render(False, False)

    node.setOrient((0,0,t*(2*np.pi)/5.0))
    t += dt
    time.sleep(dt)