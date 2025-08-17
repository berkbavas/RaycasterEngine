# Raycaster Engine

A modern 2D raycaster engine using compute shaders for real-time rendering, inspired by classic games like Wolfenstein 3D.

## Getting Started

### Prerequisites

- CMake 3.25.1 or newer
- Visual Studio 2022 with MSVC v143 C++ Compiler
- Qt 6.7.3 MSVC2022 64bit

### Build Instructions

1. Set `Qt6_DIR` environment variable to your Qt path (e.g. `C:\Qt\6.7.3\msvc2022_64`)
2. Clone the repository:

    ```sh
    git clone https://github.com/berkbavas/RaycasterEngine.git
    ```

3. Create and enter a build directory:

    ```sh
    mkdir Build
    cd Build
    ```

4. Run CMake:

    ```sh
    cmake ..
    ```

5. Open `RaycasterEngine.sln` in Visual Studio and build.

## Screenshot

![Screenshot](https://user-images.githubusercontent.com/53399385/210181260-9a01340c-8d6e-451c-80c5-164f749f4cb8.png)

## Credits

- Raycasting logic adapted from [Lode Vandevenne's tutorials](https://lodev.org/cgtutor/raycasting.html)
- Textures from Wolfenstein 3D

## License

This project is licensed under the [MIT License](LICENSE).
