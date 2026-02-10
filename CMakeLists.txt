cmake_minimum_required(VERSION 3.20)
project(stlish VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings (cross-platform)
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    # GCC and Clang
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    
    # Windows-specific: prevent min/max macro issues
    if(WIN32)
        add_definitions(-DNOMINMAX)
    endif()
endif()

# Header-only library
add_library(stlish INTERFACE)
target_include_directories(stlish INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Testing
option(STLISH_BUILD_TESTS "Build tests" ON)
if(STLISH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
option(STLISH_BUILD_EXAMPLES "Build examples" ON)
if(STLISH_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()