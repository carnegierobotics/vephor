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

v4.Window.setServerModeBYOC()

plt = v4.Plot()
plt.back_color((0,0,0))
plt.fore_color((0.5,0.5,0.5))
plt.grid_color((0,0,0))

N_CELLS = 20

g = v4.Grid(N_CELLS//2)
g.setColor((0.25,0.25,0.25))
plt.window().add(g)

off_cube_nodes = []
on_cube_nodes = []

for i in range(N_CELLS):
    for j in range(N_CELLS):
        cube = v4.Cube()
        cube.setColor((0.01,0.01,0.01))
        cube_node = plt.window().add(cube, np.array([i-N_CELLS//2+0.5,j-N_CELLS//2+0.5,0]))
        cube_node.setScale(0.45)
        off_cube_nodes.append(cube_node)
        
        cube = v4.Cube()
        cube.setColor((0,1,0))
        cube_node = plt.window().add(cube, np.array([i-N_CELLS//2+0.5,j-N_CELLS//2+0.5,0]))
        cube_node.setScale(0.45)
        cube_node.setShow(False)
        on_cube_nodes.append(cube_node)
    
def apply_number(num):
    for i in range(len(on_cube_nodes)):
        if i == num:
            off_cube_nodes[i].setShow(False)
            on_cube_nodes[i].setShow(True)
        else:
            on_cube_nodes[i].setShow(False)
            off_cube_nodes[i].setShow(True)

num = 0

while 1:
    apply_number(num)
    num += 1
    num %= len(on_cube_nodes)

    plt.show(False, False)
    time.sleep(0.05)
    