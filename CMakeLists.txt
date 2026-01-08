cmake_minimum_required(VERSION 3.10)
project(lab1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(inc/tmplInc inc/util src/tmpl src/utilImpl)

add_executable(lab1 main.cpp)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets LinguistTools)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS src/ui)

add_executable(lab1_qt 
    src/ui/main.cpp 
    src/ui/mainwindow.cpp 
    inc/ui/mainwindow.hpp
    src/ui/mainwindow.ui
    src/ui/translations.qrc
)

target_link_libraries(lab1_qt Qt6::Core Qt6::Widgets)
target_include_directories(lab1_qt PRIVATE inc/ui ${CMAKE_CURRENT_BINARY_DIR})

add_executable(heightMaps src/heightMaps/main.cpp)

target_compile_options(lab1 PRIVATE
    $<$<CONFIG:Debug>:
        -g
        -O0
        -Wall -Wextra -Werror
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    >
    $<$<CONFIG:Release>:-O3 -DNDEBUG -Wall -Wextra -Werror>
)

target_compile_options(lab1_qt PRIVATE
    $<$<CONFIG:Debug>:
        -g
        -O0
        -Wall -Wextra -Werror
        # -fsanitize=address,undefined
        # -fno-omit-frame-pointer
    >
    $<$<CONFIG:Release>:-O3 -DNDEBUG -Wall -Wextra -Werror>
)

target_compile_options(heightMaps PRIVATE
    $<$<CONFIG:Debug>:
        -g
        -O0
        -Wall -Wextra -Werror
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    >
    $<$<CONFIG:Release>:-O3 -DNDEBUG -Wall -Wextra -Werror>
)

target_link_options(lab1 PRIVATE
    $<$<CONFIG:Debug>:
        -fsanitize=address,undefined
    >
)

target_link_options(heightMaps PRIVATE
    $<$<CONFIG:Debug>:
        -fsanitize=address,undefined
    >
)