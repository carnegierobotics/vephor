#! /usr/bin/env python3

import os
import numpy as np

from pyv4 import *

plt = Plot()

plt.plot([0,10],[0,10])
plt.text("Bottom", 0.25, (0,0), (0,0,0))
plt.text("Top", 0.25, (10,10), (0,0,0))

plt.show()