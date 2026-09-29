function(set_project_warnings target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE
            /W4
            /WX
            /permissive-
        )
    else()
        target_compile_options(${target_name} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
            -Werror
        )
    endif()
endfunction()

function(enable_sanitizers target_name)
    if(NOT MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target_name} PRIVATE 
            -fsanitize=address,undefined 
            -fno-omit-frame-pointer
        )
        target_link_options(${target_name} PRIVATE 
            -fsanitize=address,undefined
        )
    endif()
endfunction()