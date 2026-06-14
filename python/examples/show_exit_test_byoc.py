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

# The point of this test is to see that the BYOC process is no longer running after this exits

import os
import numpy as np
import vephor as v4
import time

v4.Window.setServerModeBYOC()

plt = v4.Plot()

start_time = time.time()
while True:
    plt.show(wait_close=False, wait_key=False)
    time.sleep(0.03)
    if time.time() - start_time > 5.0:
        break