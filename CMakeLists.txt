cmake_minimum_required(VERSION 3.20)
project(stlish VERSION 0.1.0 LANGUAGES CXX)

option(STLISH_ENABLE_TESTS "Build tests" ON)
option(STLISH_ENABLE_BENCHMARKS "Build benchmarks" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(stlish INTERFACE)
target_include_directories(stlish INTERFACE /include)

if(STLISH_ENABLE_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()
