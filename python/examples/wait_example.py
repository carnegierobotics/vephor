#! /usr/bin/env python3

import os
import numpy as np

import vephor as v4

v4.Window.setServerModeBYOC()

plt = v4.Plot("Clock")
plt.equal()

plt.limits(-1,1,-1,1)

angle = 0
while True:
    plt.plot([0,np.cos(angle)],[0,np.sin(angle)],thickness=0.01)
    if not plt.show(wait_key = True):
        break
    angle += 0.2
    plt.clear()