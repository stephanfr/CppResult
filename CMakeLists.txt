cmake_minimum_required(VERSION 3.14.0)
project(CppResult VERSION 0.1.0)

# Determine if fmt is built as a subproject (using add_subdirectory)
# or if it is the master project.
if (NOT DEFINED CPPRESULT_IS_MASTER_PROJECT)
  set(CPPRESULT_IS_MASTER_PROJECT OFF)
  if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CPPRESULT_IS_MASTER_PROJECT ON)
    message(STATUS "CMake version: ${CMAKE_VERSION}")
    message(STATUS "CppResult is master project")
  endif ()
endif ()

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

include(FetchContent)

FetchContent_Declare(
    codecoverage
    GIT_REPOSITORY "https://github.com/bilke/cmake-modules.git" )

FetchContent_Declare(
    formatlib
    GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
    GIT_TAG "7.1.3")

FetchContent_MakeAvailable(codecoverage)
FetchContent_MakeAvailable(formatlib)

include_directories(
  ${formatlib_SOURCE_DIR}/include
  ${formatlib_BIN_DIR}
  include
  )

set(CMAKE_MODULE_PATH ${codecoverage_SOURCE_DIR})

enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

include(CodeCoverage)
append_coverage_compiler_flags()

setup_target_for_coverage_lcov(NAME cpp_result_tests_coverage EXECUTABLE cpp_result_tests DEPENDENCIES cpp_result_tests EXCLUDE "/usr/*" "${PROJECT_SOURCE_DIR}/build/*" )

if( CPPRESULT_IS_MASTER_PROJECT )
  add_subdirectory("unit_tests")
endif()