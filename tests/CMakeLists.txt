cmake_minimum_required(VERSION 3.10)
project(lab1_tests)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

include_directories(
    ${CMAKE_SOURCE_DIR}/../inc/tmplInc
    ${CMAKE_SOURCE_DIR}/../inc/util
    ${CMAKE_SOURCE_DIR}/../src/tmpl
    ${CMAKE_SOURCE_DIR}/../src/utilImpl
)

add_executable(SmartPtrTests SmartPtrTests.cpp)
target_link_libraries(SmartPtrTests ${GTEST_LIBRARIES} pthread)

add_executable(LazySequenceTests LazySequenceTests.cpp)
target_link_libraries(LazySequenceTests ${GTEST_LIBRARIES} pthread)

enable_testing()
add_test(NAME SmartPtrTests COMMAND SmartPtrTests)
add_test(NAME LazySequenceTests COMMAND LazySequenceTests)

target_compile_options(SmartPtrTests PRIVATE
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)

target_compile_options(LazySequenceTests PRIVATE
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)
