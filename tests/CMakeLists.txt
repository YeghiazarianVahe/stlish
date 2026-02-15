# file: tests/CMakeLists.txt
# ============================================================================
# stlish test suite
#
# The test framework is self-contained inside array_test.cpp — it defines its
# own main() and assertion macros.  No Catch2 or other external library is
# needed.
#
# CTest integration:
#   The executable is registered as a single CTest test.  The binary exits
#   with code 0 on full success and 1 if any check fails, which is exactly
#   what CTest expects.
#
#   Run with:
#     ctest --test-dir <build_dir> --output-on-failure
# ============================================================================

add_executable(stlish_tests
    array_test.cpp
)

# Inherit C++ standard, warnings, and sanitizers from the library target.
target_link_libraries(stlish_tests PRIVATE stlish::stlish)

# Register with CTest as a single test entry.
# COMMAND names the executable; CTest checks its exit code.
add_test(
    NAME    array_tests
    COMMAND stlish_tests
)

# Treat a non-zero exit code as a test failure (default CTest behaviour, but
# made explicit here for documentation purposes).
set_tests_properties(array_tests PROPERTIES
    FAIL_REGULAR_EXPRESSION "FAIL"   # also catch unexpected stderr/stdout
    PASS_REGULAR_EXPRESSION "0 failed"
)