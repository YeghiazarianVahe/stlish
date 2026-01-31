include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# pick up all tests/*.cpp automatically
file(GLOB STLISH_TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(stlish_tests ${STLISH_TEST_SOURCES})

# HARD FIX: ensure include path is correct for tests
target_include_directories(stlish_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(stlish_tests PRIVATE GTest::gtest_main stlish)

include(GoogleTest)
gtest_discover_tests(stlish_tests)
