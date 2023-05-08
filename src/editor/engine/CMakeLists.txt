# configure target
set(ARCTIC_ENGINE_VERSION_MAJOR 0)
set(ARCTIC_ENGINE_VERSION_MINOR 1)
set(ARCTIC_ENGINE_VERSION_PATCH 0)

# create target
set(TARGET ArcticEngine)
message("target is ${TARGET}")
add_library(${TARGET} STATIC)

# set variables
set(INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include)
set(INCLUDE_DIRS_INTERNAL ${INCLUDE_DIRS}/engine)
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

# set properties
set_target_properties(${TARGET} PROPERTIES
        INCLUDE_DIRS ${INCLUDE_DIRS})

# set sources
target_sources(${TARGET}
        PUBLIC
        ${INCLUDE_DIRS_INTERNAL}/arctic_engine.h
        PRIVATE
        ${SRC_DIR}/arctic_engine.cpp
        ${SRC_DIR}/renderloop.cpp
        ${SRC_DIR}/renderpipeline.cpp
        ${SRC_DIR}/swapchain.cpp
        ${SRC_DIR}/vulkan_loader.cpp
        ${SRC_DIR}/vulkan_window.cpp
        )

# set includes
target_include_directories(${TARGET}
        PRIVATE
        ${INCLUDE_DIRS})

# link packages
FindPackage_Vulkan(${TARGET})
FindPackage_GLFW(  ${TARGET})
FindPackage_GLM(   ${TARGET})
FindPackage_FMT(   ${TARGET})

# add module: x11
find_package(X11 REQUIRED)
target_link_libraries(${TARGET} PRIVATE X11::X11_xcb)

# add module: utilities
target_link_libraries(${TARGET} PRIVATE Utilities X11::X11_xcb)
get_target_property(Utilties_INCLUDE_DIRS Utilities INCLUDE_DIRS)
target_include_directories(${TARGET} PRIVATE ${Utilties_INCLUDE_DIRS})