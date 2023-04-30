cmake_minimum_required(VERSION 3.19)
project(ex1_sadna)

set(CMAKE_CXX_STANDARD 14)

add_executable(ex1_sadna main.cpp server.cpp server.h client.cpp client.h)