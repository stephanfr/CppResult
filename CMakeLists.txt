cmake_minimum_required(VERSION 3.14.0)
project(CppResult VERSION 0.1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
    formatlib
    GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
    GIT_TAG "7.1.3")

FetchContent_MakeAvailable(formatlib)

include_directories(
  ${formatlib_SOURCE_DIR}/include
  ${formatlib_BIN_DIR}
  include
  )

enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_subdirectory("unit_tests")
