include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(stlish_smoke_test test_smoke.cpp)
target_link_libraries(stlish_smoke_test PRIVATE GTest::gtest_main stlish)

include(GoogleTest)
gtest_discover_tests(stlish_smoke_test)
