#! /usr/bin/env python3

from pyv4 import *
import numpy as np

w = Window()

points = []
for i in range(0,100):
    for j in range(0,100):
        points.append((i,j,0))
points = np.array(points)

p = Particle(points)
p.setScreenSpaceMode()
p.setSize(0.001)
w.add(p)
w.save("test_scene")