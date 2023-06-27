# function to find glfw
function(FindPackage_SDL TARGET_NAME)

    find_package(SDL2 REQUIRED)
    target_link_libraries(${TARGET_NAME} PRIVATE ${SDL2_LIBRARIES})
    target_include_directories(${TARGET_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})

endfunction()