project(tests)

set(TEST_FILES
    wordle_tests.cpp
)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(GTest::GTest INTERFACE IMPORTED)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include(GoogleTest)

add_executable(test_wordle ${TEST_FILES})
target_link_libraries(test_wordle ${APPLICATION_LIBRARY} GTest::gtest_main)

gtest_discover_tests(test_wordle WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../)
