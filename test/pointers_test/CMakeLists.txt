

cmake_minimum_required(VERSION 3.31)
set(CMAKE_CXX_STANDARD 20)

get_target_property(GSL_INCLUDE_DIRS GSL INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(POINTERS_INCLUDE_DIRS pointers_library INTERFACE_INCLUDE_DIRECTORIES)

add_executable(pointers_tests 
    maybe_null_tests.cpp
    not_null_tests.cpp
    borrower_tests.cpp 
    owner_tests.cpp 
    exception_tests.cpp
    owner_tests.cpp) 
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

add_subdirectory(fail)

target_clangformat_setup(pointers_tests)
