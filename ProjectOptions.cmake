include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


include(CheckCXXSourceCompiles)


macro(MySDLShooter_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)

    message(STATUS "Sanity checking UndefinedBehaviorSanitizer, it should be supported on this platform")
    set(TEST_PROGRAM "int main() { return 0; }")

    # Check if UndefinedBehaviorSanitizer works at link time
    set(CMAKE_REQUIRED_FLAGS "-fsanitize=undefined")
    set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=undefined")
    check_cxx_source_compiles("${TEST_PROGRAM}" HAS_UBSAN_LINK_SUPPORT)

    if(HAS_UBSAN_LINK_SUPPORT)
      message(STATUS "UndefinedBehaviorSanitizer is supported at both compile and link time.")
      set(SUPPORTS_UBSAN ON)
    else()
      message(WARNING "UndefinedBehaviorSanitizer is NOT supported at link time.")
      set(SUPPORTS_UBSAN OFF)
    endif()
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    if (NOT WIN32)
      message(STATUS "Sanity checking AddressSanitizer, it should be supported on this platform")
      set(TEST_PROGRAM "int main() { return 0; }")

      # Check if AddressSanitizer works at link time
      set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
      set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=address")
      check_cxx_source_compiles("${TEST_PROGRAM}" HAS_ASAN_LINK_SUPPORT)

      if(HAS_ASAN_LINK_SUPPORT)
        message(STATUS "AddressSanitizer is supported at both compile and link time.")
        set(SUPPORTS_ASAN ON)
      else()
        message(WARNING "AddressSanitizer is NOT supported at link time.")
        set(SUPPORTS_ASAN OFF)
      endif()
    else()
      set(SUPPORTS_ASAN ON)
    endif()
  endif()
endmacro()

macro(MySDLShooter_setup_options)
  option(MySDLShooter_ENABLE_HARDENING "Enable hardening" ON)
  option(MySDLShooter_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    MySDLShooter_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    MySDLShooter_ENABLE_HARDENING
    OFF)

  MySDLShooter_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR MySDLShooter_PACKAGING_MAINTAINER_MODE)
    option(MySDLShooter_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(MySDLShooter_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(MySDLShooter_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(MySDLShooter_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(MySDLShooter_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(MySDLShooter_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(MySDLShooter_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(MySDLShooter_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(MySDLShooter_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(MySDLShooter_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(MySDLShooter_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(MySDLShooter_ENABLE_PCH "Enable precompiled headers" OFF)
    option(MySDLShooter_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(MySDLShooter_ENABLE_IPO "Enable IPO/LTO" ON)
    option(MySDLShooter_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(MySDLShooter_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(MySDLShooter_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(MySDLShooter_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(MySDLShooter_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(MySDLShooter_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(MySDLShooter_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(MySDLShooter_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(MySDLShooter_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(MySDLShooter_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(MySDLShooter_ENABLE_PCH "Enable precompiled headers" OFF)
    option(MySDLShooter_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      MySDLShooter_ENABLE_IPO
      MySDLShooter_WARNINGS_AS_ERRORS
      MySDLShooter_ENABLE_USER_LINKER
      MySDLShooter_ENABLE_SANITIZER_ADDRESS
      MySDLShooter_ENABLE_SANITIZER_LEAK
      MySDLShooter_ENABLE_SANITIZER_UNDEFINED
      MySDLShooter_ENABLE_SANITIZER_THREAD
      MySDLShooter_ENABLE_SANITIZER_MEMORY
      MySDLShooter_ENABLE_UNITY_BUILD
      MySDLShooter_ENABLE_CLANG_TIDY
      MySDLShooter_ENABLE_CPPCHECK
      MySDLShooter_ENABLE_COVERAGE
      MySDLShooter_ENABLE_PCH
      MySDLShooter_ENABLE_CACHE)
  endif()

  MySDLShooter_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED AND (MySDLShooter_ENABLE_SANITIZER_ADDRESS OR MySDLShooter_ENABLE_SANITIZER_THREAD OR MySDLShooter_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(MySDLShooter_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(MySDLShooter_global_options)
  if(MySDLShooter_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    MySDLShooter_enable_ipo()
  endif()

  MySDLShooter_supports_sanitizers()

  if(MySDLShooter_ENABLE_HARDENING AND MySDLShooter_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR MySDLShooter_ENABLE_SANITIZER_UNDEFINED
       OR MySDLShooter_ENABLE_SANITIZER_ADDRESS
       OR MySDLShooter_ENABLE_SANITIZER_THREAD
       OR MySDLShooter_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${MySDLShooter_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${MySDLShooter_ENABLE_SANITIZER_UNDEFINED}")
    MySDLShooter_enable_hardening(MySDLShooter_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(MySDLShooter_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(MySDLShooter_warnings INTERFACE)
  add_library(MySDLShooter_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  MySDLShooter_set_project_warnings(
    MySDLShooter_warnings
    ${MySDLShooter_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(MySDLShooter_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    MySDLShooter_configure_linker(MySDLShooter_options)
  endif()

  include(cmake/Sanitizers.cmake)
  MySDLShooter_enable_sanitizers(
    MySDLShooter_options
    ${MySDLShooter_ENABLE_SANITIZER_ADDRESS}
    ${MySDLShooter_ENABLE_SANITIZER_LEAK}
    ${MySDLShooter_ENABLE_SANITIZER_UNDEFINED}
    ${MySDLShooter_ENABLE_SANITIZER_THREAD}
    ${MySDLShooter_ENABLE_SANITIZER_MEMORY})

  set_target_properties(MySDLShooter_options PROPERTIES UNITY_BUILD ${MySDLShooter_ENABLE_UNITY_BUILD})

  if(MySDLShooter_ENABLE_PCH)
    target_precompile_headers(
      MySDLShooter_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(MySDLShooter_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    MySDLShooter_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(MySDLShooter_ENABLE_CLANG_TIDY)
    MySDLShooter_enable_clang_tidy(MySDLShooter_options ${MySDLShooter_WARNINGS_AS_ERRORS})
  endif()

  if(MySDLShooter_ENABLE_CPPCHECK)
    MySDLShooter_enable_cppcheck(${MySDLShooter_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(MySDLShooter_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    MySDLShooter_enable_coverage(MySDLShooter_options)
  endif()

  if(MySDLShooter_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(MySDLShooter_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(MySDLShooter_ENABLE_HARDENING AND NOT MySDLShooter_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR MySDLShooter_ENABLE_SANITIZER_UNDEFINED
       OR MySDLShooter_ENABLE_SANITIZER_ADDRESS
       OR MySDLShooter_ENABLE_SANITIZER_THREAD
       OR MySDLShooter_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    MySDLShooter_enable_hardening(MySDLShooter_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
