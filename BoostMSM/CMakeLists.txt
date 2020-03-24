set(CMAKE_CXX_STANDARD 14)


add_executable (FSM_boost_msm_test test/test_poc_boostmsm.cpp)
target_link_libraries(FSM_boost_msm_test gtest_main)
add_test(FSM_boost_msm_test FSM_boost_msm_test)