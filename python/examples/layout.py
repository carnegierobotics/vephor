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
import time
import vephor as v4

v4.Window.setServerModeBYOC()

def make_window():
    w=v4.Window()

    w.add(v4.Axes(), (500,-1000,2000))
    w.add(v4.Sphere(), (495,-1000,2000))
    w.add(v4.Cone(), (505,-1000,2000))
    w.add(v4.Cylinder(), (500,-995,2000))
    w.add(v4.Cube(), (500,-1005,2000))

    return w

windows = [
    make_window(),
    make_window(),
    make_window()
]

windows[0].layoutPerUnit(0.5,0.5,0.0,0.0)
windows[1].layoutPerUnit(0.5,0.5,0.0,0.5)
windows[2].layoutPerUnit(0.5,1.0,0.5,0.0)

while True:
    for w in windows:
        w.render(False)
    time.sleep(0.01)