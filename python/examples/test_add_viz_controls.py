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

import os
import numpy as np
import time
import vephor as v4

metadata = ShowMetadata()
metadata.app_name = "test_add_viz_controls"
metadata.flags["Add Window"].toggle = False
metadata.flags["Add Plot"].toggle = False
v4.Window.setServerMode(port=8921, metadata=metadata)

while True:
    time.sleep(0.01)