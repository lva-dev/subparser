cmake_minimum_required(VERSION 3.10)
project(subparser)

option(SUBPARSER_BUILD_EXAMPLES "set to ON to build examples. OFF by default" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SUBPARSER_TARGET ${PROJECT_NAME})

add_library(${SUBPARSER_TARGET} INTERFACE
    include/subparser/subparser.h
)

target_include_directories(${SUBPARSER_TARGET} INTERFACE include)

target_compile_options(${SUBPARSER_TARGET} INTERFACE
    -Wall
    -Wextra
    -pedantic
)

if (SUBPARSER_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()