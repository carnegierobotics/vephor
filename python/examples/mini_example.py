#! /usr/bin/env python3

import os

import vephor as v4

w=v4.Window()

w.add(v4.Axes(), (500,-1000,2000))
w.add(v4.Sphere(), (495,-1000,2000))
w.add(v4.Cone(), (505,-1000,2000))
w.add(v4.Cylinder(), (500,-995,2000))
w.add(v4.Cube(), (500,-1005,2000))

w.render()