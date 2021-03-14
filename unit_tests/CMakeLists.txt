
find_package(Catch2 REQUIRED)
add_executable(tests ResultTest.cpp )
target_include_directories( tests PRIVATE ../src )
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain fmt)

include(CTest)
include(Catch)
catch_discover_tests(tests)
