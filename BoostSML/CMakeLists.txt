set(CMAKE_CXX_STANDARD 14)

include_directories("include")

add_executable (boost_sml_test test/test_boost_sml.cpp test/test_fsm_in_class.cpp)
target_link_libraries(boost_sml_test gtest_main)
add_test(boost_sml_test boost_sml_test)
