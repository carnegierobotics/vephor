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

import vephor as v4

plt = v4.Plot()

plt.circle((-10,-10), 3.0, np.array((1.0,0.0,0.0)))
plt.circle((-10,10), 3.0, np.array((255,0,0)))
plt.circle((10,10), 3.0, (255,0,0))
plt.circle((10,-10), 3.0, (1.0,0.0,0.0))

plt.show()