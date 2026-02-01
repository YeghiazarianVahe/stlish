# ========================================
# Helper function to create example executables
# ========================================
function(add_stlish_example example_name)
  add_executable(${example_name} ${example_name}.cpp)
  target_link_libraries(${example_name} PRIVATE stlish)
  
  # Set output directory
  set_target_properties(${example_name} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
  )
endfunction()

# ========================================
# Vector Examples
# ========================================
add_stlish_example(demo_vector_basics)
add_stlish_example(demo_vector_perf)

# ========================================
# Forward List Examples
# ========================================
add_stlish_example(forward_list_demo)
