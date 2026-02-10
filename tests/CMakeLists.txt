# Fetch Catch2
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.1
)
FetchContent_MakeAvailable(Catch2)

# Test executable
add_executable(stlish_tests
    array_test.cpp
)

target_link_libraries(stlish_tests PRIVATE
    stlish
    Catch2::Catch2WithMain
)

# Register with CTest
include(CTest)
include(Catch)
catch_discover_tests(stlish_tests)