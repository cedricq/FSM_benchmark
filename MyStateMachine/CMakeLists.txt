set(CMAKE_CXX_STANDARD 14)

include_directories("inc")

add_executable (fsm_test src/NameString.cpp src/StateMachine.cpp test/StateMachine_t.cpp)
target_link_libraries(fsm_test gtest_main)
if(ENABLE_COVERAGE)
    target_link_libraries(fsm_test gcov)
endif()
add_test(fsm_test fsm_test)
