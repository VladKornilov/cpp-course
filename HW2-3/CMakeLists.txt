cmake_minimum_required(VERSION 3.9)
project(HW2_3)

set(CMAKE_CXX_STANDARD 14)

add_executable(HW2_3
        #main.cpp
        big_integer.cpp
        big_integer.h
        big_integer_testing.cpp
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc
        vector.cpp
        vector.h
        )

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
endif()

target_link_libraries(HW2_3 -lpthread)