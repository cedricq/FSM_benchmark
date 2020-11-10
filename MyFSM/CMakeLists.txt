set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable (myfsm_test test/test_myfsm.cpp test/fsm-t.cc fsm.cc)
target_link_libraries(myfsm_test gtest_main)
add_test(myfsm_test myfsm_test)
