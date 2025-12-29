cmake_minimum_required(VERSION 3.10)
project(lab1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(inc/tmplInc inc/util src/tmpl src/utilImpl)

add_executable(lab1 main.cpp)

target_compile_options(lab1 PRIVATE
    $<$<CONFIG:Debug>:
        -g
        -O0
        -Wall -Wextra -Werror
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    >
    $<$<CONFIG:release>:-O3 -DNDEBUG -Wall -Wextra -Werror>
)

target_link_options(lab1 PRIVATE
    $<$<CONFIG:Debug>:
        -fsanitize=address,undefined
    >
)