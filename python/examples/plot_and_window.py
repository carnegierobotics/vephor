#! /usr/bin/env python3

#
# Copyright 2026
# Carnegie Robotics, LLC
# 4501 Hatfield Street, Pittsburgh, PA 15201
# https://www.carnegierobotics.com
#
# This source code is licensed under the Apache License, Version 2.0
# found in the LICENSE file in the root directory of this source tree.
#

import os
import numpy as np
import time
import vephor as v4

v4.Window.setServerMode(port=8921)
#v4.Window.setServerModeBYOC()

plt = v4.Plot()

plt.plot([0,10],[0,10])
plt.text("Bottom", 0.25, (0,0), (0,0,0))
plt.text("Top", 0.25, (10,10), (0,0,0))


plt2 = v4.Plot()

plt3 = v4.Plot()


w=v4.Window()

w.add(v4.Axes(), (500,-1000,2000))
w.add(v4.Sphere(), (495,-1000,2000))
w.add(v4.Cone(), (505,-1000,2000))
w.add(v4.Cylinder(), (500,-995,2000))
w.add(v4.Cube(), (500,-1005,2000))

w.render()


t = 0
while w.render(False) and plt.show(False) and plt2.show(False) and plt3.show(False):
    plt2.clear()
    x = np.arange(100)
    plt2.plot(x,np.sin(x*0.1+t))

    plt3.clear()
    x = np.arange(100)
    plt3.plot(x,np.cos(x*0.1+t))

    t += 0.01
    time.sleep(0.01)