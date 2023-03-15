# configure cmake version
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# configure cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Arctic
        LANGUAGES "CXX")

add_executable(ArcticEngine2
        main.cpp)
