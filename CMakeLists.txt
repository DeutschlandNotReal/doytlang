cmake_minimum_required(VERSION 3.22)

project(doytlang)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(doytlang INTERFACE)

target_include_directories(doytlang INTERFACE include)

add_executable(cli
    cli/main.cpp
)

target_link_libraries(cli
    PRIVATE doytlang
)

install(TARGETS cli
    RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)
