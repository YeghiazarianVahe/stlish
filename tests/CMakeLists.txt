# ========================================
# Helper function to create test executables
# ========================================
function(add_stlish_test test_name)
  add_executable(${test_name} ${test_name}.cpp)
  target_link_libraries(${test_name} PRIVATE
      stlish
      Catch2::Catch2WithMain
  )
  
  # Discover tests automatically
  include(Catch)
  catch_discover_tests(${test_name})
endfunction()

# ========================================
# Forward List Tests (Catch2)
# ========================================
add_stlish_test(test_forward_list)

# ========================================
# Vector Tests (TODO: Migrate to Catch2)
# ========================================
# Temporarily disabled - need migration from GoogleTest to Catch2
# add_stlish_test(test_vector_api)
# add_stlish_test(test_vector_constructors)
# add_stlish_test(test_vector_iterators)
# add_stlish_test(test_vector_exceptions)
# add_stlish_test(test_vector_invariants)
# add_stlish_test(test_vector_clear_lifetime)