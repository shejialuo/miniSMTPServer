enable_testing()

add_executable(
  stateTest
  stateTest.cpp
)

target_include_directories(stateTest PRIVATE ../)

target_link_libraries(
  stateTest
  context
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(stateTest)