cmake_minimum_required(VERSION 3.27)
project(paca-tools)

# Set C++ standard and optimization level
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG -Wall -Wpedantic")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

include(${CMAKE_CURRENT_SOURCE_DIR}/serializer-generator.cmake)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)
