#! /usr/bin/env python3

import os
import numpy as np
import time

import vephor as v4

v4.Window.setServerModeBYOC()

second_hand = False

def key_press_handler(key_code):
    global second_hand 
    print("Key code:", key_code)
    if key_code == 82:
        second_hand = not second_hand

plt = v4.Plot("Clock")
plt.window().setKeyPressCallback(key_press_handler)
plt.equal()

plt.limits(-1,1,-1,1)

angle = 0
angle_2 = 0
while True:
    plt.plot([0,np.cos(angle)],[0,np.sin(angle)],thickness=0.01)
    if second_hand:
        plt.plot([0,np.cos(angle_2)],[0,np.sin(angle_2)],thickness=0.01)
    if not plt.show(wait_close = False, wait_key = False):
        break
    angle += 0.1
    angle_2 += 0.25
    plt.clear()
    time.sleep(0.03)