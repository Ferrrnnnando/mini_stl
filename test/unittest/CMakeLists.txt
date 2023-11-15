file(GLOB_RECURSE CPP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(mini-test ${CPP_SOURCES})

target_link_libraries(mini-test
    PRIVATE
    mini-lib
    mini-unittest-main
    GTest::GTest
)
include(GoogleTest)
gtest_discover_tests(mini-test)
