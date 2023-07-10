# configure target
set(ARCTIC_RENDERING_VERSION_MAJOR 0)
set(ARCTIC_RENDERING_VERSION_MINOR 1)
set(ARCTIC_RENDERING_VERSION_PATCH 0)

# create target
set(TARGET ArcticRendering)
message("target is ${TARGET}")
add_library(${TARGET} STATIC)

# set variables
set(INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

# set properties
set_target_properties(
    ${TARGET} 
    PROPERTIES
        INCLUDE_DIR ${INCLUDE_DIR})

# set sources
target_sources(
    ${TARGET}
    PRIVATE
        ${SRC_DIR}/uniform_buffer_object.cpp
        ${SRC_DIR}/vertex.cpp
    )

# set includes
target_include_directories(
    ${TARGET}
    PRIVATE
        ${INCLUDE_DIR}
    )

# link packages
FindPackage_GLM(${TARGET})