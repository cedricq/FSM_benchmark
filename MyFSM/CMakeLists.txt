
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER "g++" )
set(CMAKE_C_COMPILER "gcc" )
set(CMAKE_CXX_FLAGS "-isystem -mconsole -pthread")

set(GTEST_PATH "${CMAKE_CURRENT_SOURCE_DIR}/gtest")

project(myfsm)

include_directories(${GTEST_PATH}/include ${CMAKE_CURRENT_SOURCE_DIR})

add_executable (myfsm_test test/test_myfsm.cpp myfsm.hpp ${GTEST_PATH}/include/gtest/gtest.h)

target_link_libraries(myfsm_test ${GTEST_PATH}/build/libgtest_main.a ${GTEST_PATH}/build/libgtest.a)

add_test(myfsm_test myfsm_test)
