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

metadata = v4.ShowMetadata()
metadata.app_name = "test_add_viz_controls"
metadata.setFlag("Add Window", False)
metadata.setFlag("Add Plot", False)
v4.Window.setServerMode(port=8921, show_metadata=metadata)

windows = []
plots = []

exit = False
while not exit:
    if v4.Window.checkAndConsumeFlag("Add Window"):
        windows.append(v4.Window(1000,1000,f"Window {len(windows)}"))
    if v4.Window.checkAndConsumeFlag("Add Plot"):
        plots.append(v4.Plot(f"Plot {len(plots)}"))

    for w in windows:
        if not w.render(False):
            exit = True

    for p in plots:
        if not p.show(False):
            exit = True

    time.sleep(0.01)