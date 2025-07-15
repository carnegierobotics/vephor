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
import time

import vephor as v4

v4.Window.setServerModeBYOC()

second_hand = False

def key_press_handler(key_code):
    global second_hand 
    print("Key code:", key_code)
    if key_code == 82: # Key 'R'
        second_hand = not second_hand

plt = v4.Plot("Clock")
plt.window().setKeyPressCallback(key_press_handler)
plt.equal()

plt.limits(-1,1,-1,1)

angle = 0
angle_2 = 0
while True:
    plt.plot([0, np.cos(angle)], [0, np.sin(angle)], thickness_in_screen_perc=0.01)
    if second_hand:
        plt.plot([0, np.cos(angle_2)], [0, np.sin(angle_2)], thickness_in_screen_perc=0.01)
    if not plt.show(wait_close = False, wait_key = False):
        break
    angle += 0.1
    angle_2 += 0.25
    plt.clear()
    time.sleep(0.03)