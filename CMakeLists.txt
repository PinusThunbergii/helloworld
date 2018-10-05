cmake_minimum_required(VERSION 3.8)

project(Render CXX)

include_directories(src/tgaimage)
include_directories(src/geometry)
include_directories(src/model)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()    
if(MSVC)
	add_definitions(-DWINDOWS_BUILD)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
	add_subdirectory(src/getopt)
	include_directories(src/getopt)
endif()

message(STATUS "${CMAKE_CXX_FLAGS}")

add_subdirectory(src/model)
add_subdirectory(src/tgaimage)
add_subdirectory(src/geometry)

set(SOURCE_EXE src/main.cpp)
add_executable(main ${SOURCE_EXE})

if(MSVC)
	target_link_libraries(main getopt)
endif()
target_link_libraries(main tgaimage geometry model)