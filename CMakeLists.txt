cmake_minimum_required(VERSION 3.20)
project(stlish VERSION 0.1.0 LANGUAGES CXX)

option(STLISH_ENABLE_TESTS "Build tests" ON)
option(STLISH_ENABLE_BENCHMARKS "Build benchmarks" OFF)
option(STLISH_ENABLE_EXAMPLES "Build examples" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /WX-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ========================================
# Fetch Catch2 (for testing)
# ========================================
if(STLISH_ENABLE_TESTS)
  include(FetchContent)
  
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.0
    GIT_SHALLOW    TRUE
  )
  
  FetchContent_MakeAvailable(Catch2)
  
  # Add Catch2 CMake modules to path
  list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
endif()

# ========================================
# Header-only library
# ========================================
add_library(stlish INTERFACE)

# IMPORTANT: makes #include "stlish/vector.hpp" work
target_include_directories(stlish
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(stlish INTERFACE cxx_std_20)

# ========================================
# Subdirectories
# ========================================
if(STLISH_ENABLE_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()

# if(STLISH_ENABLE_EXAMPLES)
#   add_subdirectory(examples)
# endif()