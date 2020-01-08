cmake_minimum_required(VERSION 3.0)
project(POS_Klient)

set(CMAKE_CXX_STANDARD 14)

add_executable(POS_Klient main2.cpp Figurka.cpp Figurka.h)
target_link_libraries( POS_Klient pthread )