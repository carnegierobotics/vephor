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

### Viz by server

The following code will open a server on the default Vephor port, 5533.  The port can be set when enabling server mode.

In addition to server use, this example also shows how to allow the client to continue updating the visualization.  The visualization will run separately from any client being connected, and a new client will begin visualizing at the current state of the client.

Note that multiple clients can connect to one server.

```
#include "vephor_ext.h"

using namespace vephor;

int main()
{
	Window window;
	
	window.setServerMode();
	
	double obj_mesh_dist = 5.0f;
	
	
	// Make a "clock"
	for (int i = 1; i <= 12; i++)
	{
		float angle = i / 12.0 * 2 * M_PI;
		
		Vec3 dir = Vec3(cos(angle), sin(angle), 0);
		auto text = make_shared<Text>(std::to_string(i));
		text->setAnchorCentered();
		window.add(text, dir * obj_mesh_dist * 1.5);
		
		vector<Vec3> verts = {
			Vec3(0,0,0),
			dir * obj_mesh_dist * 1.5
		};
		window.add(make_shared<Lines>(verts), Vec3::Zero());
	}
	
	// Use a mesh for the clock hand
	string base_asset_dir = getBaseAssetDir();
	auto obj_mesh = make_shared<ObjMesh>(base_asset_dir+"/assets/pyramid.obj");
	auto obj_mesh_node = window.add(obj_mesh, Transform3(Vec3(obj_mesh_dist,0,0)));
	
	float angle = 0;
	while (true)
	{
		angle += 0.01;
		obj_mesh_node->setPos(Vec3(cos(angle), sin(angle), 0) * obj_mesh_dist);
		obj_mesh_node->setOrient(Vec3(0,0,angle+M_PI));
		
		window.render(false /*wait*/);
		
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(33)));
	}

	return 0;
}
```

Once this server is running, connect to it using:

```
vephor_show -m client
```

### Viz by server (bring your own client)

"Bring your own client" mode allows you to create interactive visualizations without needing to separately call vephor_show.  It will create a server and also create a client process to talk to that server.

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

