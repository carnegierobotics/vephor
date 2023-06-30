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