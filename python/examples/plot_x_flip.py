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
plt.xflip()
plt.equal()

# Create a 16x16 blank image
img = np.zeros((16, 16), dtype=np.uint8)

# Draw a "F"
img[15,:] = 255
img[9,:] = 255
img[:,0] = 255

plt.plot([[-0.5,0.5],[-0.5,15.5]])
plt.plot([[-0.5,9.5],[-15,9.5]])
plt.plot([[-0.5,15.5],[-15,15.5]])
plt.text("This is an F", 0.5, [-14,8], [0,1,0])
plt.imshow(img, offset=[-16,0], nearest=True, no_flip=True)

plt.show()