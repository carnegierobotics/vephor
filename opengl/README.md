Installation:

Linux:

Git
```
sudo apt install git
```

GLEW
```
sudo apt-get install libglew-dev
```

GLFW
```
Download source from https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.zip
Unzip and enter the root
mkdir build
cd build
cmake ..
make
sudo make install
```

Vephor
```
Download Vephor source
Follow install process in README
```

Vephor OpenGL
```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
sudo make install
```

Windows:

MSYS2
```
www.msys2.org
Install msys2 to c:/msys64
Open MSYS2 shell
pacman -Sy pacman
Re-open shell
pacman -S mingw-w64-x86_64-toolchain, install all
pacman -S make
Add C:\msys64\mingw64\bin to the environment path
```

CMake
Install cmake using an installer such as https://github.com/Kitware/CMake/releases/download/v3.26.0-rc4/cmake-3.26.0-rc4-windows-x86_64.msi

Git
Download and run https://github.com/git-for-windows/git/releases/download/v2.39.2.windows.1/Git-2.39.2-64-bit.exe

GLEW
```
Download source from http://glew.sourceforge.net/index.html
Unzip, navigate to build/cmake/
cmake . -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=c:/msys64/mingw64
mingw32-make all
mingw32-make install
```

GLFW
```
Download source from https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.zip
Unzip and enter the root
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=c:/msys64/mingw64
mingw32-make all
mingw32-make install
```

Vephor
```
Download Vephor source
Follow install process in README
```

Vephor OpenGL
```
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=c:/msys64/mingw64
mingw32-make -j7 all
```