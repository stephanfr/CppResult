cmake_minimum_required(VERSION 3.14.0)
project(CppResult VERSION 0.1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

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

if( CPPRESULT_IS_MASTER_PROJECT )
  add_subdirectory("unit_tests")
endif()
