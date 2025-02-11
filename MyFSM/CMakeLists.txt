
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER "g++" )
set(CMAKE_C_COMPILER "gcc" )
set(CMAKE_CXX_FLAGS "-isystem -mconsole -std=c++20 -fpermissive -pthread")

set(GTEST_PATH "${CMAKE_CURRENT_SOURCE_DIR}/gtest")

project(FiniteStateMachine)

include_directories(${GTEST_PATH}/include ${CMAKE_CURRENT_SOURCE_DIR}/inc )

add_executable (fsm_test test/StateMachine_t.cpp src/StateMachine.cpp inc/StateMachine.h src/NameString.cpp inc/NameString.h  ${GTEST_PATH}/include/gtest/gtest.h)

target_link_libraries(fsm_test ${GTEST_PATH}/build/libgtest_main.a ${GTEST_PATH}/build/libgtest.a)

add_test(fsm_test fsm_test)
