cmake_minimum_required (VERSION 2.6)
project (AlgEngProject1)

add_library(TestFramework test.cpp test.h)

add_executable(Project1Naive naive.cpp)
add_executable(Project1BTree blocked.cpp)

target_link_libraries (Project1Naive TestFramework)
target_link_libraries (Project1BTree TestFramework)