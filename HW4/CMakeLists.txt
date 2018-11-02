cmake_minimum_required(VERSION 3.9)
project(HW4)

set(CMAKE_CXX_STANDARD 11)

add_executable(HW4 ui/main.cpp
                   src/filework.cpp
                   src/huffman.cpp src/huffman.h)
