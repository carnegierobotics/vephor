import os

# Hack to support MinGW
if os.name == "nt":
    os.add_dll_directory("C:/msys64/mingw64/bin")

from ._core import *