cmake_minimum_required (VERSION 2.6)
project (AlgEngProject2)

string(COMPARE EQUAL ${CMAKE_SYSTEM_NAME} "Linux" LINUX)

IF(${LINUX})
  set(CMAKE_CXX_COMPUILER "clang++")
  set(CMAKE_BUILD_TYPE Release)
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

  add_library(
    ipcm_lib
    STATIC
    EXCLUDE_FROM_ALL
    /ipcm/msr.o
    /ipcm/pci.o
    /ipcm/cpucounters.o
  )

  set_target_properties(
    ipcm_lib
    PROPERTIES
    LINKER_LANGUAGE CXX
  )
ENDIF(${LINUX})

IF(APPLE)
  set(CMAKE_CXX_COMPUILER "clang++")
  set(CMAKE_BUILD_TYPE Release)
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
ENDIF(APPLE)

add_executable(Project2 main.cpp
                        matrix.h
                        layouts.h
                        matrixmul.h
                        test.h)
target_link_libraries(Project2 pthread)
IF(${LINUX})
  target_link_libraries(Project2 ipcm_lib rt)
ENDIF(${LINUX})
