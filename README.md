# ArcticEngine2

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