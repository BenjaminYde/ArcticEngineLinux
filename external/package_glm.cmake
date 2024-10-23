# function to find glm
function(FindPackage_GLM TARGET_NAME)
    FetchContent_Declare(external_glm
            GIT_REPOSITORY    https://github.com/g-truc/glm
            GIT_TAG           1.0.1)

    FetchContent_MakeAvailable(external_glm)

    target_link_libraries(${TARGET_NAME} PRIVATE glm::glm)
endfunction()