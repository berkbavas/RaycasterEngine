# 2D Raycaster Engine
Traditional 2D raycaster engines run on CPU so they have a bottleneck on loading screen texture from CPU to GPU for each frame.

This raycaster engine is implemented using compute shaders so it does not have this bottleneck as it runs on GPU.

Textures belong to `Wolfenstein 3D`.

Raycaster codes are taken from [Lode Vandevenne](https://lodev.org/)'s  [tutorials](https://lodev.org/cgtutor/raycasting.html) and adopted.

## Build
1) Install `CMake 3.25.1` or above.
2) Install `Visual Studio 2022` and `MSVC C++ v143 Compiler`.
3) Install `Qt 5.x.y MSVC2019 64bit` kit.
4) Set environment variable `Qt5_DIR` as `C:\Qt\5.x.y\msvc2019_64`.
5) Clone the repo `git clone https://github.com/berkbavas/RaycasterEngine.git`.
6) Create a folder `mkdir Build`.
7) Enter the folder `cd Build`.
8) Run CMake `cmake ..`.
9) Open `RaycasterEngine.sln` with `Visual Studio 2022`.
10) Build & Run with `Release` config.

## Screenshot
![Screenshot_00](https://user-images.githubusercontent.com/53399385/210181260-9a01340c-8d6e-451c-80c5-164f749f4cb8.png)

## Keywords
`2D Raycaster`,
`2D Rendering`,
`Compute Shaders`,
`OpenGL`,
`GLSL`,
`Wolfenstein 3D`.
