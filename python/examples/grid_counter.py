#! /usr/bin/env python3

#
# Copyright 2023
# Carnegie Robotics, LLC
# 4501 Hatfield Street, Pittsburgh, PA 15201
# https://www.carnegierobotics.com
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Carnegie Robotics, LLC nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL CARNEGIE ROBOTICS, LLC BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
    