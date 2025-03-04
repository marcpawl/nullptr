

cmake_minimum_required(VERSION 3.31)
set(CMAKE_CXX_STANDARD 20)

get_target_property(GSL_INCLUDE_DIRS GSL INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(POINTERS_INCLUDE_DIRS pointers_library INTERFACE_INCLUDE_DIRECTORIES)
# Function to test compilation failure
function(test_compile_failure test_name source_file expected_error)
    # Try to compile the source file
    try_compile(
        COMPILE_RESULT
        ${CMAKE_BINARY_DIR}/compile_tests/${test_name}
	SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${source_file}
        CMAKE_FLAGS 
	  "-DCMAKE_CXX_FLAGS=-Werror"
	  "-DINCLUDE_DIRECTORIES=${POINTERS_INCLUDE_DIRS};${GSL_INCLUDE_DIRS}"
        OUTPUT_VARIABLE COMPILE_OUTPUT
    )

    # Check if the compilation failed as expected
    if(COMPILE_RESULT)
	    message(FATAL_ERROR "Test ${test_name} failed: Compilation succeeded unexpectedly.  Expecting: '${expected_error}'")
    else()
        # Optionally, check if the expected error message is in the output
        if(NOT "${COMPILE_OUTPUT}" MATCHES "${expected_error}")
		message(FATAL_ERROR "Test ${test_name} failed: Expected error message ${source_file} '${expected_error}' not found in output.\n${COMPILE_OUTPUT}")
        else()
		message(STATUS "Test ${test_name} passed: Compilation failed as expected with error ${source_file}")
        endif()
    endif()
endfunction()

# Run the test
test_compile_failure(
	"TestCompilationFailure" 
         maybe_null_must_be_pointer.cpp
         "error: static assertion failed due to requirement 'gsl::details::is_comparable_to_nullptr<int, void>::value': T cannot be compared to nullptr." )
test_compile_failure(
	"TestCompilationFailure" 
	"maybe_null_visit_bad_nullptr_handler.cpp"
        "handle_nullptr:auto = \\(lambda at")
test_compile_failure(
	"TestCompilationFailure" 
	"maybe_null_visit_bad_not_null_handler.cpp"
        "handle_not_null:auto = \\(lambda at")
test_compile_failure(
	"TestCompilationFailure" 
	"maybe_null_deprecated.cpp"
	"'get' is deprecated")
test_compile_failure(
	"TestCompilationFailure" 
	"maybe_null_deprecated.cpp"
	"'operator->' is deprecated")
test_compile_failure(
	"TestCompilationFailure" 
        "maybe_null_deprecated.cpp"
	"'operator\\*' is deprecated")
test_compile_failure(
	"TestCompilationFailure" 
	"borrower_must_be_pointer.cpp"
	"note: because 'int' does not satisfy 'Pointer'")
#test_compile_failure("TestCompilationFailure" fail/borrower_not_deletable.cpp "'__is_invocable<marcpawl::pointers::NonDeletable")
test_compile_failure(
	"TestCompilationFailure" 
	borrower_not_null_default_contstructor.cpp
	"no matching constructor")
test_compile_failure(
	"TestCompilationFailure" 
        borrower_not_null_nullptr_constructor.cpp
	"parameter cannot be nullptr")


