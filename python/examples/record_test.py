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
import time

v4.Window.setRecordMode("/tmp/vephor_record")

w = v4.Window()
w.add(v4.Axes())
w.add(v4.Grid(10))
obj = w.add(v4.Sphere())

for i in range(1000):
    t = i/100.0
    dist = 5
    obj.setPos((np.cos(t)*dist,np.sin(t)*dist,0))
    w.render(False)
    time.sleep(0.01)