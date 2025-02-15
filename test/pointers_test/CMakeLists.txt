cmake_minimum_required(VERSION 3.31)
project(MainProject)


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
