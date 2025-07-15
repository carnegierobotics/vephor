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

plt = v4.Plot()

plt.plot([0,10],[0,10],label="First")
plt.plot([10,0],[0,10],label="Second")

plt.show()