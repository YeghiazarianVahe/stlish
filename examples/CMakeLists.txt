add_executable(demo_vector_basics demo_vector_basics.cpp)

target_link_libraries(demo_vector_basics PRIVATE stlish)

add_executable(demo_vector_perf demo_vector_perf.cpp)

target_link_libraries(demo_vector_perf PRIVATE stlish)

set_target_properties(demo_vector_basics demo_vector_perf PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)