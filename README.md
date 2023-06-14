# 2D Raycaster Engine
Traditional 2D raycaster engines run on CPU so they have a bottleneck on loading screen texture from CPU to GPU for each frame.

This raycaster engine is implemented using compute shaders so it does not have this bottleneck as it runs on GPU.

Textures belong to Wolfenstein 3D game.

Raycaster codes are taken from Lode Vandevenne's  [tutorials](https://lodev.org/cgtutor/raycasting.html) and adopted.

## Build
1) Install `CMake 3.25.1`.
2) Install `Visual Studio 2019 and MSVC C++ Compiler`.
3) Install `Qt 5.15.2 MSVC2019 64bit` kit.
4) Clone the repo `git clone https://github.com/berkbavas/RaycasterEngine.git`.
5) Create a folder `mkdir Build`.
6) Enter the folder `cd Build`.
7) Run CMake `cmake ..`.
8) Open `RaycasterEngine.sln` with `Visual Studio 2019`.
9) Build & Run with `Release` config.

## Screenshot
![Screenshot_00](https://user-images.githubusercontent.com/53399385/210181260-9a01340c-8d6e-451c-80c5-164f749f4cb8.png)
