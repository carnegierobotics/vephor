#! /usr/bin/env python3

import os

from pyv4 import *

w=Window()

w.add(Axes(), (500,-1000,2000))
w.add(Sphere(), (495,-1000,2000))
w.add(Cone(), (505,-1000,2000))
w.add(Cylinder(), (500,-995,2000))
w.add(Cube(), (500,-1005,2000))

w.render()