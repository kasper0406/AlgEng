cmake_minimum_required (VERSION 2.6)
set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
project (AlgEngProject1)

add_executable(Project1 main.cpp
                        blocked.cpp
                        blocked.h
                        naive.cpp
                        naive.h
                        test.cpp
                        test.h)
