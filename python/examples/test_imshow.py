#! /usr/bin/env python3

import os

import vephor as v4
import numpy as np

image = np.zeros((16,16,3))
image[:8,8:] = np.array((0.5,0.5,0.5))
image[8:,:8] = np.array((0.75,0.75,0.75))
image[8:,8:] = np.array((1,1,1))

plt = v4.Plot()
plt.imshow(image)
plt.show()
