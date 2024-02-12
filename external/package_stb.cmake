# function to find glm
function(FindPackage_STB TARGET_NAME)
    FetchContent_Declare(external_stb
            GIT_REPOSITORY    https://github.com/nothings/stb)


    FetchContent_GetProperties(external_stb)

    if (NOT external_stb_POPULATED)
        FetchContent_Populate(external_stb)
    endif()

    target_include_directories(${TARGET_NAME} PRIVATE ${external_stb_SOURCE_DIR})
endfunction()