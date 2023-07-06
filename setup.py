from skbuild import setup  # This line replaces 'from setuptools import setup'
setup(
    name="vephor",
    version="0.1.0",
    description="Vephor, a visualization library",
    author='Steve Landers',
    packages=['vephor'],
    package_dir={'':'python'},
    python_requires=">=3.7",
    cmake_args=["-DVEPHOR_BUILD_PYTHON_BINDINGS:BOOL=ON"],
)