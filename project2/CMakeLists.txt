cmake_minimum_required (VERSION 2.6)
project (AlgEngProject2)

IF(LINUX)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
ENDIF(LINUX)

add_executable(Project2 main.cpp)
