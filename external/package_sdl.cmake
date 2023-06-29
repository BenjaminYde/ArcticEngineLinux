# function to find glfw
function(FindPackage_SDL TARGET_NAME)

    find_package(SDL2 REQUIRED)
    target_link_libraries(${TARGET_NAME} PRIVATE SDL2::SDL2)

endfunction()