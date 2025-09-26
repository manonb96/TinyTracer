## Tiny tracer
A ray tracing application built from the ground up. 
Its a work in progress and more features will be added over the time to come!

### Features
- CPU Ray Tracing with BVH as acceleration structure
- Texture rendering using OpenGL or Vulkan
- Building with CMake

### Next steps
- Improving lighting
- Building a pretty scene
- Camera transformations with OpenGL and Vulkan
- GPU ray tracing
- Rendering meshes
- Post processing
- GUI

### Building
To build the project, I am using [CMake](https://cmake.org/) (version 3.5 or higher) with [Ninja](https://ninja-build.org/) as the build system generator and the Microsoft Visual C++ (MSVC) compiler and linker,
targeting 64-bit Windows.

It can be build from Visual Studio IDE directly, or from the command line using the Developer Command Prompt for Visual Studio 2022 with the following commands:

1. Set up environment to 64-bit version by running <code>vcvars64.bat</code>. Example:

    ```sh
    "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
    ```

2. Navigate to the Project folder:
    
    ```sh   
    cd path/to/TinyTracer/Project
    ```
 
2. Configure project using CMake preset to use [Ninja](https://ninja-build.org/) generator (if installed):

    ```sh
    cmake --preset x64-Debug
    ```

3. Navigate to the build folder:
    
    ```sh   
    cd out/build/x64-Debug
    ```

4. Build the project:

    ```sh
    cmake --build .
    ```

### Dependencies
This project uses the following packages:
- [Vulkan](https://www.lunarg.com/vulkan-sdk/)

### References
- Joey de Vries' [LearnOpenGL-book](https://learnopengl.com/) to set up OpenGL
- Hristo Lliev's [Vulkan course](https://www.udemy.com/course/graphics-programming-with-vulkan-and-cpp) to set up Vulkan