#! /usr/bin/env python3

from pyv4 import *
import numpy as np

w = Window()
w.add(Sphere(),(5,0,0))
w.add(Sphere(),(-5,0,0))
w.add(Sphere(),(0,5,0))
w.add(Sphere(),(0,-5,0))
w.add(Cylinder(),(0,0,5))
w.add(Cylinder(),(0,0,-5))

path = []
for i in range(100000):
    t = i/10000.0
    ang = t*2*np.pi
    path.append([np.cos(ang)*3,np.sin(ang)*3,t-5])

l = Lines(path)
l.setColor([1,0,0])
w.add(l,np.zeros(3))
w.render()