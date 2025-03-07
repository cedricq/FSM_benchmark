cmake_minimum_required(VERSION 3.14)
project(my_fsm_projects)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER "g++" )
set(CMAKE_C_COMPILER "gcc" )
set(CMAKE_CXX_FLAGS "-isystem -mconsole -std=c++20 -fpermissive -pthread")
#set(CMAKE_CXX_STANDARD_REQUIRED ON)



option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        message(STATUS "Coverage enabled with MinGW/GCC")
        add_compile_options(--coverage -O0 -g)
        add_link_options(--coverage)
    else()
        message(WARNING "Coverage only supported with MinGW/GCC on Windows")
    endif()
endif()


include(CTest)

add_subdirectory (TinyFSM)
add_subdirectory (BoostSML)
#add_subdirectory (BoostMSM)
#add_subdirectory (BoostEUML)
add_subdirectory (MyStateMachine)