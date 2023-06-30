Installation:


Linux:

Git
sudo apt install git

Eigen
Download from https://gitlab.com/libeigen/eigen/-/releases
Unzip, enter directory
sudo cp -r Eigen /usr/local/include/

Manif
git clone https://github.com/artivis/manif.git
mkdir build
cd build
cmake ..
sudo make install

Vephor
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
sudo make install


Windows:

MSYS2
www.msys2.org
Install msys2 to c:/msys64
Open MSYS2 shell
pacman -Sy pacman
Re-open shell
pacman -S mingw-w64-x86_64-toolchain, install all
pacman -S make
Add C:\msys64\mingw64\bin to the environment path

CMake
Install cmake using an installer such as https://github.com/Kitware/CMake/releases/download/v3.26.0-rc4/cmake-3.26.0-rc4-windows-x86_64.msi

Git
Download and run https://github.com/git-for-windows/git/releases/download/v2.39.2.windows.1/Git-2.39.2-64-bit.exe

Eigen
Download from https://gitlab.com/libeigen/eigen/-/releases
Unzip, enter directory
cp -r Eigen C:\msys64\mingw64\include\

Manif
git clone https://github.com/artivis/manif.git
cp -r include/manif C:\msys64\mingw64\include\
cp -r external/tl/tl C:\msys64\mingw64\include\

Vephor
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=c:/msys64/mingw64
mingw32-make -j7 all