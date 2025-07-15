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
import numpy as np

import vephor as v4

v4.Window.setServerModeBYOC()

plt = v4.Plot("Clock")
plt.equal()

plt.limits(-1,1,-1,1)

angle = 0
while True:
    plt.plot([0,np.cos(angle)],[0,np.sin(angle)],thickness_in_screen_perc=1.0)
    if not plt.show(wait_key = True):
        break
    angle += 0.2
    plt.clear()