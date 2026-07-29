cmake_minimum_required(VERSION 3.20)
project(shard VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wconversion
        -Wsign-conversion
    )
endif()

# Build type default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Source files
file(GLOB_RECURSE SHARD_SOURCES
    src/core/*.cpp
    src/lexer/*.cpp
    src/parser/*.cpp
    src/ast/*.cpp
    src/commands/*.cpp
    src/executor/*.cpp
    src/filesystem/*.cpp
    src/help/*.cpp
    src/autocomplete/*.cpp
    src/utils/*.cpp
)

add_executable(shard src/main.cpp ${SHARD_SOURCES})

target_include_directories(shard PRIVATE src)

# Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE TEST_SOURCES tests/*.cpp)

if(TEST_SOURCES)
    add_executable(shard_tests ${TEST_SOURCES} ${SHARD_SOURCES})
    target_include_directories(shard_tests PRIVATE src)
    target_link_libraries(shard_tests GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(shard_tests)
endif()
