#! /usr/bin/env python3

from pyv4 import *
import numpy as np
import time

Window.setServerModeBYOC()

w = Window()

w.add(Grid(10))

N_BITS = 8

off_cube_nodes = []
on_cube_nodes = []

for i in range(N_BITS):
    cube = Cube()
    cube.setColor((1,0,0))
    cube_node = w.add(cube, np.array([i,0,0]))
    cube_node.setScale(0.45)
    off_cube_nodes.append(cube_node)
    
    cube = Cube()
    cube.setColor((0,1,0))
    cube_node = w.add(cube, np.array([i,0,0]))
    cube_node.setScale(0.45)
    cube_node.setShow(False)
    on_cube_nodes.append(cube_node)
    
def apply_number(num):
    mask = 1
    for i in range(N_BITS):
        if num & mask:
            off_cube_nodes[i].setShow(False)
            on_cube_nodes[i].setShow(True)
        else:
            on_cube_nodes[i].setShow(False)
            off_cube_nodes[i].setShow(True)
        mask *= 2

auto_inc = True
num = 0
def key_press_handler(key_code):
    global auto_inc
    global num
    if key_code == 265: # up arrow
        auto_inc = False
        num += 1
        num %= 256
    elif key_code == 264: # down arrow
        auto_inc = False
        num -= 1
        num %= 256

w.setKeyPressCallback(key_press_handler)

while 1:
    apply_number(num)
    if auto_inc:
        num += 1
        num %= 256

    w.render(False, False)
    time.sleep(0.05)
    