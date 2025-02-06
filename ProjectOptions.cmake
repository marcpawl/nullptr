include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


macro(nullptr_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)
    set(SUPPORTS_UBSAN ON)
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    set(SUPPORTS_ASAN ON)
  endif()
endmacro()

macro(nullptr_setup_options)
  option(nullptr_ENABLE_HARDENING "Enable hardening" ON)
  option(nullptr_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    nullptr_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    nullptr_ENABLE_HARDENING
    OFF)

  nullptr_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR nullptr_PACKAGING_MAINTAINER_MODE)
    option(nullptr_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(nullptr_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(nullptr_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(nullptr_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(nullptr_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(nullptr_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(nullptr_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(nullptr_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(nullptr_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(nullptr_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(nullptr_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(nullptr_ENABLE_PCH "Enable precompiled headers" OFF)
    option(nullptr_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(nullptr_ENABLE_IPO "Enable IPO/LTO" ON)
    option(nullptr_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(nullptr_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(nullptr_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(nullptr_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(nullptr_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(nullptr_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(nullptr_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(nullptr_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(nullptr_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(nullptr_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(nullptr_ENABLE_PCH "Enable precompiled headers" OFF)
    option(nullptr_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      nullptr_ENABLE_IPO
      nullptr_WARNINGS_AS_ERRORS
      nullptr_ENABLE_USER_LINKER
      nullptr_ENABLE_SANITIZER_ADDRESS
      nullptr_ENABLE_SANITIZER_LEAK
      nullptr_ENABLE_SANITIZER_UNDEFINED
      nullptr_ENABLE_SANITIZER_THREAD
      nullptr_ENABLE_SANITIZER_MEMORY
      nullptr_ENABLE_UNITY_BUILD
      nullptr_ENABLE_CLANG_TIDY
      nullptr_ENABLE_CPPCHECK
      nullptr_ENABLE_COVERAGE
      nullptr_ENABLE_PCH
      nullptr_ENABLE_CACHE)
  endif()

  nullptr_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED AND (nullptr_ENABLE_SANITIZER_ADDRESS OR nullptr_ENABLE_SANITIZER_THREAD OR nullptr_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(nullptr_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(nullptr_global_options)
  if(nullptr_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    nullptr_enable_ipo()
  endif()

  nullptr_supports_sanitizers()

  if(nullptr_ENABLE_HARDENING AND nullptr_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR nullptr_ENABLE_SANITIZER_UNDEFINED
       OR nullptr_ENABLE_SANITIZER_ADDRESS
       OR nullptr_ENABLE_SANITIZER_THREAD
       OR nullptr_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${nullptr_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${nullptr_ENABLE_SANITIZER_UNDEFINED}")
    nullptr_enable_hardening(nullptr_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(nullptr_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(nullptr_warnings INTERFACE)
  add_library(nullptr_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  nullptr_set_project_warnings(
    nullptr_warnings
    ${nullptr_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(nullptr_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    nullptr_configure_linker(nullptr_options)
  endif()

  include(cmake/Sanitizers.cmake)
  nullptr_enable_sanitizers(
    nullptr_options
    ${nullptr_ENABLE_SANITIZER_ADDRESS}
    ${nullptr_ENABLE_SANITIZER_LEAK}
    ${nullptr_ENABLE_SANITIZER_UNDEFINED}
    ${nullptr_ENABLE_SANITIZER_THREAD}
    ${nullptr_ENABLE_SANITIZER_MEMORY})

  set_target_properties(nullptr_options PROPERTIES UNITY_BUILD ${nullptr_ENABLE_UNITY_BUILD})

  if(nullptr_ENABLE_PCH)
    target_precompile_headers(
      nullptr_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(nullptr_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    nullptr_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(nullptr_ENABLE_CLANG_TIDY)
    nullptr_enable_clang_tidy(nullptr_options ${nullptr_WARNINGS_AS_ERRORS})
  endif()

  if(nullptr_ENABLE_CPPCHECK)
    nullptr_enable_cppcheck(${nullptr_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(nullptr_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    nullptr_enable_coverage(nullptr_options)
  endif()

  if(nullptr_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(nullptr_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(nullptr_ENABLE_HARDENING AND NOT nullptr_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR nullptr_ENABLE_SANITIZER_UNDEFINED
       OR nullptr_ENABLE_SANITIZER_ADDRESS
       OR nullptr_ENABLE_SANITIZER_THREAD
       OR nullptr_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    nullptr_enable_hardening(nullptr_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
