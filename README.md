# ArcticEngine

Development Environment:

- Linux
- Clang
- CMake
- VS Code using Dev Containers (Docker)

Libraries used: 

- Vulkan SDK
- SDL2
- GLM
- FMT

# CMake Graph Visualizer

Use `graphviz` to visualize the dependency graph:

1. Navigate to your build directory
1. Run CMake with the --graphviz option
    - This will generate a graph.dot file in your build directory 
1. Use dot (included in graphviz) to generate an image from the .dot file:
    - This will generate a graph.png image file in your build directory.

```sh
cd /path/to/your/build
cmake --graphviz=graph.dot .
dot -Tpng graph.dot -o graph.png
```
# Vulkan Test

To verify that vulkan works correctly,
run the following command:

```sh
vulkaninfo
```

and

```sh
vkvia
```

# Renderdoc

1. Open the terminal 
1. Enter the following command to open renderdoc:
    ```bash
    qrenderdoc
    ```
1. Inside the app, a warning should tell you that vulkan needs to be configured for renderdoc
   Click on that warning, that enables you to generate the config. Make sure to generate in the user and not root.


# Build Shaders

`glslc` is a command-line tool that is part of the Shaderc project. It's essentially a compiler that transforms GLSL (OpenGL Shading Language) source code into SPIR-V binary format.

To use glsc, execute the following command format:

```bash
glslc <path_to_shader> -o <path_of_build>
```

For example: 

```bash
glslc shader.vert -o vert.spv
glslc.exe shader.frag -o frag.spv
```

In these examples, `shader.vert` is the vertex shader and `shader.frag` is the fragment shader. The `-o` option specifies the output file, which will be in SPIR-V format.

Afterward, you can load these `.spv` files in your Vulkan application and create shader modules from them, which can then be used in the graphics pipeline.