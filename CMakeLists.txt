cmake_minimum_required(VERSION 3.22)

project(doytlang)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(doytlang SHARED
        logic/executer.cpp
        logic/parser.cpp
        logic/lexer.cpp
)

target_include_directories(doytlang PUBLIC include)

set_target_properties(doytlang PROPERTIES POSITION_INDEPENDENT_CODE ON)


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