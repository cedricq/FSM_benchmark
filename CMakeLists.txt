# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)
project (FSM_1)


SET(GCC_PROJECT_COMPILE_FLAGS "-std=c++14 -O2 -fno-exceptions -Wall -Wextra -Werror -pedantic -pedantic-errors")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

#add_library (FSM_1_lib main.cpp)
#target_include_directories (FSM_1_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable (FSM_1 main.cpp)


add_subdirectory (TinyFSM)
add_subdirectory (BoostSML)
