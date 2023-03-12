#! /usr/bin/env python3

import os

from pyv4 import *
import numpy as np
import time

Window.setClientMode()

w=Window()

node = w.add(np.zeros(3))
w.add(Axes(), np.zeros(3))
w.add(Grid(5), np.zeros(3))
for i in range(16):
    ang = 2*np.pi/16*i
    w.add(Sphere(0.25), (np.cos(ang)*5,np.sin(ang)*5,0)).setParent(node)

t = 0
dt = 0.015
while 1:
    w.render()

    node.setOrient((0,0,t*(2*np.pi)/5.0))
    t += dt
    time.sleep(dt)