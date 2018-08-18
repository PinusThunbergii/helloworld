cmake_minimum_required(VERSION 3.8)

project(Render CXX)
include_directories(src/tgaimage)
include_directories(src/geometry)
include_directories(src/model)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src/model)
add_subdirectory(src/tgaimage)
add_subdirectory(src/geometry)


set(SOURCE_EXE src/main.cpp)

message(STATUS ${CMAKE_CXX_FLAGS})

add_executable(main ${SOURCE_EXE})

target_link_libraries(main tgaimage geometry model)
