# configure target
set(ARCTIC_ENGINE_VERSION_MAJOR 0)
set(ARCTIC_ENGINE_VERSION_MINOR 1)
set(ARCTIC_ENGINE_VERSION_PATCH 0)

# create target
set(TARGET ArcticEngine)
message("target is ${TARGET}")
add_library(${TARGET} STATIC)

# set variables
set(INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

# set properties
set_target_properties(${TARGET} PROPERTIES
        INCLUDE_DIR ${INCLUDE_DIR})

# set sources
target_sources(${TARGET}
        PUBLIC
        ${INCLUDE_DIR}/engine/arctic_engine.h
        PRIVATE
        ${SRC_DIR}/engine/arctic_engine.cpp
        )

# set includes
target_include_directories(${TARGET}
        PRIVATE
        ${INCLUDE_DIR}
        )

FindPackage_SDL(  ${TARGET})
FindPackage_Vulkan(${TARGET})

# add module: arctic vulkan
target_link_libraries(
        ${TARGET} 
        PRIVATE 
        ArcticVulkan)

get_target_property(
        ArcticVulkan_INCLUDE_DIR
        ArcticVulkan
        INCLUDE_DIR)

target_include_directories(
        ${TARGET} 
        PRIVATE 
        ${ArcticVulkan_INCLUDE_DIR})