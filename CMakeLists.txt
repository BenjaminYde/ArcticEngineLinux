# configure cmake version
cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

# cmake include modules
include(FetchContent)
include(CMakePrintHelpers)

# configure cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Arctic
        LANGUAGES "CXX")

#add_executable(Arctic main.cpp)
#return()

#cmake_print_variables(CMAKE_CXX_COMPILER_ID)
#cmake_print_variables(CMAKE_CXX_COMPILER_VERSION)
#cmake_print_variables(CMAKE_CXX_COMPILER)

# check platform
if(NOT UNIX)
    # throw error: no other OS supported
    message(FATAL_ERROR "Project must be compiled on Unix!")
endif()

# check compiler: force use clang (for the moment)
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 16.0.4)
        message(FATAL_ERROR "Clang compiler version is smaller then 16.0.4! Current compiler version is ${CMAKE_CXX_COMPILER_VERSION}")
    endif()
else()
        message(FATAL_ERROR "Project must be compiled using preset! Current compiler is ${CMAKE_CXX_COMPILER_ID}")
endif()

# c++ flags
#add_compile_options(-Wshadow)

# defines
add_definitions(-DARCTIC_ASSETS_DIR="${CMAKE_CURRENT_LIST_DIR}/assets")

# set vulkan flags
add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
add_definitions(-DGLFW_EXPOSE_NATIVE_X11)

# add sub directories
add_subdirectory(external)
add_subdirectory(src)