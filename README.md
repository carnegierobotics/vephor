# vephor

## A header-only headless visualization library with an OpenGL backend

<div align="center">
  <img src="assets/screenshot.png"/>
</div>

## Summary

## Install instructions

### Linux

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=c:/msys64/mingw64
make
sudo make install

### Windows

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
mingw32-make
sudo mingw32-make install