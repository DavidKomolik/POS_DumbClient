cmake_minimum_required(VERSION 3.0)
project(DumbClient)

set(CMAKE_CXX_STANDARD 14)

add_executable(DumbClient main.cpp Figurka.cpp Figurka.h)
target_link_libraries( DumbClient pthread )