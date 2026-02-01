cmake_minimum_required(VERSION 3.20)
project(stlish VERSION 0.1.0 LANGUAGES CXX)

option(STLISH_ENABLE_TESTS "Build tests" ON)
option(STLISH_ENABLE_BENCHMARKS "Build benchmarks" ON)
option(STLISH_ENABLE_EXAMPLES "Build examples" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Header-only library
add_library(stlish INTERFACE)

# IMPORTANT: makes #include "stlish/vector.hpp" work
target_include_directories(stlish
  INTERFACE
    ${PROJECT_SOURCE_DIR}/include
)

if (STLISH_ENABLE_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()

# if (STLISH_ENABLE_BENCHMARKS)
#   add_subdirectory(benchmarks)
# endif()

if (STLISH_ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()
