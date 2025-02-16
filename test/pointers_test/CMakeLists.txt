

# Define a function to compile and check for a specific error
function(compile_and_check SOURCE_FILE ERROR_MESSAGE)
  # Compile the source file and redirect stderr to a file
  execute_process(
    COMMAND 
     ${CMAKE_CXX_COMPILER} 
     -I${CMAKE_CURRENT_SOURCE_DIR}/../../src/pointers_library/include 
     -I${CMAKE_BINARY_DIR}/_deps/gsl-src/include/
     -c 
     -o failed_program ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}
    ERROR_FILE ${CMAKE_CURRENT_BINARY_DIR}/compile_errors.txt
    RESULT_VARIABLE result
  )

  # Read the compile errors
  file(READ ${CMAKE_CURRENT_BINARY_DIR}/compile_errors.txt compile_errors)

  # Check for the specific error message
  if("${compile_errors}" MATCHES "${ERROR_MESSAGE}")
    message(STATUS "Compilation failed with the specific error as expected.")
  else()
    message(FATAL_ERROR "Compilation did not fail with the specific error.")
  endif()
endfunction()

compile_and_check(borrower_must_be_pointer.cpp "T Must be pointer")

add_executable(borrower_tests borrower_tests.cpp)
target_link_libraries(
  borrower_tests
  PRIVATE nullptr::nullptr_warnings
          nullptr::nullptr_options
          pointers_library
          Catch2::Catch2WithMain)

if(WIN32 AND BUILD_SHARED_LIBS)
  add_custom_command(
    TARGET borrower_tests
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:tests> $<TARGET_FILE_DIR:tests>
    COMMAND_EXPAND_LISTS)
endif()

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
catch_discover_tests(
  borrower_tests
  TEST_PREFIX
  "unittests."
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "unittests."
  OUTPUT_SUFFIX
  .xml)
