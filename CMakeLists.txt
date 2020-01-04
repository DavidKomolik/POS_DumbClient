cmake_minimum_required(VERSION 3.0)
project(DumbClient)

set(CMAKE_CXX_STANDARD 14)

add_executable(DumbClient main.cpp)
target_link_libraries( DumbClient pthread )