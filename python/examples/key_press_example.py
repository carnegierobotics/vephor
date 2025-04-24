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