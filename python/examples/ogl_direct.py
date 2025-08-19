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

import vephor as v4

asset_dir = v4.getBaseAssetDir()+"/assets"

w = v4.ogl.Window()
w.setFrameLock(60)

s = v4.ogl.Sphere()
w.add(s, (0,0,-20))

ambient_light = v4.ogl.AmbientLight((0.3,0.3,0.4))
w.add(ambient_light)

dir_light = v4.ogl.DirLight((0,-1,0), 0.5)
w.add(dir_light)

cube_md = v4.formCube()
m = v4.ogl.Mesh(cube_md)
m.setTexture(w.loadTexture(asset_dir+"/maze.png", False))
w.add(m, (3,-5,-20))

print(cube_md.verts)
print(cube_md.uvs)

skybox = v4.ogl.Skybox(w.getCubeTextureFromDir(asset_dir+"/Nalovardo", False))
w.add(skybox)

while w.render():
    pass