# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)
project (FSM_1)

#add_library (FSM_1_lib main.cpp)
#target_include_directories (FSM_1_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable (FSM_1 main.cpp)


add_subdirectory (TinyFSM)
add_subdirectory (BoostSML)
