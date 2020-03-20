set(CMAKE_CXX_STANDARD 14)


add_executable (FSM_boost_sml_test test/test_poc_boostsml.cpp)
target_link_libraries(FSM_boost_sml_test gtest_main)
add_test(FSM_boost_sml_test FSM_boost_sml_test)