# vephor

## A header-only headless visualization library with an OpenGL backend

<div align="center">
  <img src="assets/screenshot.png"/>
</div>

## Summary

Vephor allows for interactive visualizations in situations where visualizations are infeasible, such as on headless systems, or where it is inconvenient to include graphics libraries.  

Vephor creates graphics library agnostic visualization definitions before either saving them to disk for later viewing or sending them to a visualizer elsewhere for immediate viewing.  

When running in server mode, Vephor visualization code is meant to be as dormant as possible, only using resources when a client connects and requests the visualization.

Vephor supports both C++ and Python.

## Install instructions

Follow the instructions in README.md for vephor and vephor_opengl to install dependencies.

### Linux

```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
sudo make install
```

For Python:

```
cd vephor_python
python setup.py install
```

### Windows

```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=c:/msys64/mingw64
mingw32-make
sudo mingw32-make install
```

For Python:

```
cd vephor_python
python setup.py install -G "MinGW Makefiles"
```

## API - TLDR



## Examples

Note that the following examples alternate between C++ and Python, but all features are available in both.

### Viz by process creation

This is the default mode for viewing Vephor visualizations - your visualization will be written to a temporary location on disk, and a vephor_show process will be created and pointed to that location.  Note that these visualizations are not interactive apart from allowing camera movement - only network based visualizations allow the creator to respond to callbacks.

```
#include "vephor_ext.h"

using namespace vephor;

int main()
{
	Window window;
	
	window.add(make_shared<Axes>(), Vec3(500,-1000,2000));
	window.add(make_shared<Sphere>(), Vec3(495,-1000,2000));
	window.add(make_shared<Cone>(), Vec3(505,-1000,2000));
	window.add(make_shared<Cylinder>(), Vec3(500,-995,2000));
	window.add(make_shared<Cube>(), Vec3(500,-1005,2000));
	
	window.render();

	return 0;
}
```

### Viz by saving to a file

```
#! /usr/bin/env python3

from pyv4 import *
import numpy as np

w = Window()

points = []
for i in range(0,100):
    for j in range(0,100):
        points.append((i,j,0))
points = np.array(points)

p = Particle(points)
p.setScreenSpaceMode()
p.setSize(0.001)
w.add(p)
w.save("test_scene")
```

This will create a folder called test_scene.  The visualization can then be viewed by calling:

```
vephor_show test_scene
```

### Viz by server (basic)

### Viz by server (interactive)

### Viz by server (bring your own client)

### Transform tree

### 2d plotting

### Image plotting

```
#! /usr/bin/env python3

import os

from pyv4 import *
import numpy as np

image = np.zeros((16,16,3))
image[:8,8:] = np.array((0.5,0.5,0.5))
image[8:,:8] = np.array((0.75,0.75,0.75))
image[8:,8:] = np.array((1,1,1))

plt = Plot()
plt.imshow(image)
plt.show()
```

### Image plotting - OpenCV images in C++

## API - Exhaustive

Here are, as compactly as possible, all the relevant functions this library provides.

