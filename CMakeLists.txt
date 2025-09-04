cmake_minimum_required(VERSION 3.22)

project(doytlang)


add_library(doytlang SHARED
        logic/executer.cpp
        logic/parser.cpp
        logic/parser.cpp
        logic/lexer.cpp
        include/lexer.h
)

target_include_directories(doytlang PUBLIC include)

add_executable(cli
    cli/main.cpp
)

target_link_libraries(cli
        PRIVATE doytlang)