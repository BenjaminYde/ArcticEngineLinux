# function to find fmt
function(FindPackage_FMT TARGET_NAME)
    FetchContent_Declare(external_fmt
            GIT_REPOSITORY    https://github.com/fmtlib/fmt
            GIT_TAG           10.2.1)

    FetchContent_MakeAvailable(external_fmt)

    target_link_libraries(${TARGET_NAME} PRIVATE fmt::fmt-header-only)
endfunction()