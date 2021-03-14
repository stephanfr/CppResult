
find_package(Catch2 REQUIRED)
add_executable(cpp_result_tests ResultTest.cpp )
target_include_directories( cpp_result_tests PRIVATE ../src )
target_link_libraries(cpp_result_tests PRIVATE Catch2::Catch2WithMain fmt)

include(CTest)
include(Catch)
catch_discover_tests(cpp_result_tests)
