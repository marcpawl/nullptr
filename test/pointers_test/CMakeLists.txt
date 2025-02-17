

cmake_minimum_required(VERSION 3.14)
project(CompilationFailureTest)

# Function to test compilation failure
function(test_compile_failure test_name source_file expected_error)
    # Try to compile the source file
    try_compile(
        COMPILE_RESULT
        ${CMAKE_BINARY_DIR}/compile_tests/${test_name}
	SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${source_file}
        COMPILE_DEFINITIONS "-DERROR_MESSAGE=\"${expected_error}\""
        CMAKE_FLAGS "-DCMAKE_CXX_FLAGS=-Werror"  # Treat warnings as errors
        OUTPUT_VARIABLE COMPILE_OUTPUT
    )

    # Check if the compilation failed as expected
    if(COMPILE_RESULT)
        message(FATAL_ERROR "Test ${test_name} failed: Compilation succeeded unexpectedly.")
    else()
        # Optionally, check if the expected error message is in the output
        if(NOT "${COMPILE_OUTPUT}" MATCHES "${expected_error}")
            message(FATAL_ERROR "Test ${test_name} failed: Expected error message '${expected_error}' not found in output.")
        else()
            message(STATUS "Test ${test_name} passed: Compilation failed as expected with error '${expected_error}'.")
        endif()
    endif()
endfunction()

# Run the test
test_compile_failure("TestCompilationFailure" borrower_must_be_pointer.cpp "T Must be pointer")

add_executable(pointers_tests borrower_tests.cpp owner_tests.cpp) 
target_link_libraries(
  pointers_tests
  PRIVATE nullptr::nullptr_warnings
          nullptr::nullptr_options
          pointers_library
          Catch2::Catch2WithMain)

if(WIN32 AND BUILD_SHARED_LIBS)
  add_custom_command(
    TARGET pointers_tests
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:tests> $<TARGET_FILE_DIR:tests>
    COMMAND_EXPAND_LISTS)
endif()

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
catch_discover_tests(
  pointers_tests
  TEST_PREFIX
  "unittests."
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "unittests."
  OUTPUT_SUFFIX
  .xml)
