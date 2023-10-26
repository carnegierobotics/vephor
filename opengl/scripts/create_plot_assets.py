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

import cv2
import numpy as np
import sys

asset_path = sys.argv[1]

img_size = 256
h_img_size = img_size / 2

def in_plane(pt, norm, pt_on):
    c = np.dot(norm, -np.array(pt_on))
    return np.dot(pt, norm) + c > 0

img = np.zeros((img_size,img_size,4))
for i in range(img_size):
    for j in range(img_size):
        pt = np.array((i,j))
        if in_plane(pt, (1,1),   (0  ,h_img_size)) and \
           in_plane(pt, (-1,-1), (256,h_img_size)) and \
           in_plane(pt, (1,-1),  (0  ,h_img_size)) and \
           in_plane(pt, (-1,1),  (256,h_img_size)):
            img[i,j] = (255,255,255,255)
cv2.imwrite(asset_path+"/diamond.png", img)

img = np.zeros((img_size,img_size,4))
for i in range(img_size):
    for j in range(img_size):
        pt = np.array((i,j))
        dist = np.linalg.norm(pt - np.array([h_img_size,h_img_size]))
        if dist < h_img_size:
            img[i,j] = (255,255,255,255)
cv2.imwrite(asset_path+"/circle.png", img)

img = np.zeros((img_size,img_size,4))
img[img_size//4:3*img_size//4,:] = (255,255,255,255)
img[:,img_size//4:3*img_size//4] = (255,255,255,255)
cv2.imwrite(asset_path+"/plus.png", img)