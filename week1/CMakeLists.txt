cmake_minimum_required (VERSION 2.6)
project (AlgEngProject1)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lpapi")

add_executable(Project1 main.cpp
                        blocked.cpp
                        blocked.h
                        naive.cpp
                        naive.h
                        test.cpp
                        test.h)
