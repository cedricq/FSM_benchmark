set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable (tinyfsm_test test/test_tinyfsm.cpp)
target_link_libraries(tinyfsm_test gtest_main)
add_test(tinyfsm_test tinyfsm_test)
