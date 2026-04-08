cmake_minimum_required(VERSION 3.22)
project(doytlang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

file(GLOB_RECURSE src
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/cli/*.cpp
)

add_executable(doytlang ${src})