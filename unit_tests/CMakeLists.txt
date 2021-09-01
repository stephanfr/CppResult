
include(FetchContent)

FetchContent_Declare(
    codecoverage
    GIT_REPOSITORY "https://github.com/bilke/cmake-modules.git" )

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.0.0-preview3)
    
FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(
    formatlib
    GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
    GIT_TAG "7.1.3")

FetchContent_MakeAvailable(codecoverage)
FetchContent_MakeAvailable(formatlib)

include_directories(
  ${formatlib_SOURCE_DIR}/include
  ${formatlib_BIN_DIR}
  ../include
  )

set(CMAKE_MODULE_PATH ${codecoverage_SOURCE_DIR} ${catch2_SOURCE_DIR}/extras)

enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

include(CodeCoverage)
append_coverage_compiler_flags()

setup_target_for_coverage_lcov(NAME cpp_result_tests_coverage EXECUTABLE cpp_result_tests DEPENDENCIES cpp_result_tests EXCLUDE "/usr/*" "${PROJECT_SOURCE_DIR}/build/*" )

add_executable(cpp_result_tests ResultTest.cpp )
target_include_directories( cpp_result_tests PRIVATE ../src )
target_link_libraries(cpp_result_tests PRIVATE Catch2::Catch2WithMain fmt)

include(CTest)

include(Catch)
catch_discover_tests(cpp_result_tests)
