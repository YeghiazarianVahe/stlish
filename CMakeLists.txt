# file: CMakeLists.txt
# ============================================================================
# stlish — STL-ish learning library
#
# Build:
#   cmake -B build -DCMAKE_BUILD_TYPE=Debug
#   cmake --build build
#   ctest --test-dir build --output-on-failure
#
# To disable sanitizers explicitly (e.g. on Windows/MinGW):
#   cmake -B build -DSTLISH_SANITIZE=OFF
# ============================================================================
cmake_minimum_required(VERSION 3.20)

project(stlish
    VERSION     0.1.0
    DESCRIPTION "STL-ish: a hand-rolled STL-like library for learning C++20"
    LANGUAGES   CXX
)

# ── C++ standard ─────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)   # -std=c++20, not -std=gnu++20

# ── Compiler warnings ────────────────────────────────────────────────────────
add_library(stlish_warnings INTERFACE)
target_compile_options(stlish_warnings INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wnon-virtual-dtor>
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4 /permissive- /w14640>
)

# ── Sanitizers ───────────────────────────────────────────────────────────────
# AddressSanitizer and UBSan are supported on:
#   - Linux   (GCC and Clang)
#   - macOS   (AppleClang and Clang)
#
# They are NOT supported on:
#   - Windows MinGW  — GCC port does not ship libasan / libubsan
#   - MSVC           — uses /fsanitize=address, different mechanism entirely
#
# We therefore gate on: GCC/Clang family AND NOT Windows AND Debug config.
# The option lets you override on the command line if needed.
option(STLISH_SANITIZE "Enable AddressSanitizer + UBSan (Linux/macOS Debug only)" ON)

# Evaluate at configure time whether sanitizers can actually be used.
# CMAKE_SYSTEM_NAME is "Windows" for all Windows toolchains (MSVC, MinGW, Clang).
if(STLISH_SANITIZE AND NOT WIN32)
    set(_sanitize_supported TRUE)
else()
    set(_sanitize_supported FALSE)
    if(STLISH_SANITIZE AND WIN32)
        message(STATUS
            "stlish: STLISH_SANITIZE=ON but target is Windows — "
            "sanitizers are not available with MinGW or MSVC. "
            "Building without them. Pass -DSTLISH_SANITIZE=OFF to silence this.")
    endif()
endif()

add_library(stlish_sanitize INTERFACE)
if(_sanitize_supported)
    # Generator expression: only inject flags for GCC/Clang in Debug builds.
    # (On Linux/macOS we still respect CMAKE_BUILD_TYPE so Release stays clean.)
    target_compile_options(stlish_sanitize INTERFACE
        $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Debug>>:
            -fsanitize=address,undefined
            -fno-omit-frame-pointer>
    )
    target_link_options(stlish_sanitize INTERFACE
        $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Debug>>:
            -fsanitize=address,undefined>
    )
endif()

# ── Header-only library target ───────────────────────────────────────────────
# INTERFACE: no compiled sources; propagates include path + flags to consumers.
add_library(stlish INTERFACE)
add_library(stlish::stlish ALIAS stlish)

target_include_directories(stlish INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(stlish INTERFACE
    stlish_warnings
    stlish_sanitize
)

# ── Subdirectories ───────────────────────────────────────────────────────────
enable_testing()

add_subdirectory(tests)
add_subdirectory(examples)