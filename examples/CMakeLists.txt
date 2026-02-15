# file: examples/CMakeLists.txt
# ============================================================================
# stlish demo executables — narrative walkthroughs, not tests.
# Not registered with CTest (demos are for learning, not CI).
# ============================================================================

add_executable(array_demo array_demo.cpp)
target_link_libraries(array_demo PRIVATE stlish::stlish)