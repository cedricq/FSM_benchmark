set(CMAKE_CXX_STANDARD 14)


add_executable (boost_msm_test test/test_boost_msm.cpp)
target_link_libraries(boost_msm_test gtest_main)
add_test(boost_msm_test boost_msm_test)