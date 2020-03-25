set(CMAKE_CXX_STANDARD 14)


add_executable (boost_euml_test test/test_boost_euml.cpp)
target_link_libraries(boost_euml_test gtest_main)
add_test(boost_euml_test boost_euml_test)