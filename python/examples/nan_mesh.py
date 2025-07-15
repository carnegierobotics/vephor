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

w = v4.Window()

verts = np.zeros((6,3))
uvs = np.zeros((6,2))
norms = np.zeros((6,3))

verts[0,0] = float('nan')

data = v4.MeshData(verts, uvs, norms)
m = v4.Mesh(data)
w.add(m)

w.render()