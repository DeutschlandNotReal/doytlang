cmake_minimum_required(VERSION 3.22)

project(doytlang)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

add_library(doytlang INTERFACE)

target_include_directories(doytlang INTERFACE
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)


add_executable(cli
    cli/main.cpp
)

target_link_libraries(cli
        PRIVATE doytlang)

install(TARGETS doytlang cli
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)
install(DIRECTORY include/ DESTINATION include)