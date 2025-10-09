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

import numpy as np
import vephor as v4

v4.setTextureCompression(False)

plt = v4.Plot()
plt.darkMode()
plt.xyswap()
plt.equal()

# Create a 16x16 blank image
img = np.zeros((16, 16), dtype=np.uint8)

# Draw a "W"
for col in [0, 15]:
    img[4:, col] = 255

for i in range(4, 16):
    if i < 11:
        img[i, i - 4] = 255
    if i < 11:
        img[i, 15 - (i - 4)] = 255

img[11, 7:9] = 255

plt.plot([[16,0.5],[5,0.5],[11,8],[5,15.5],[16,15.5]])
plt.text("This is a W", 0.5, [16,8], [0,1,0])
plt.imshow(img, nearest=True, no_flip=True)

plt.show()