#! /usr/bin/env python3

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