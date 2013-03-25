cmake_minimum_required (VERSION 2.6)
project (AlgEngProject2)

string(COMPARE EQUAL ${CMAKE_SYSTEM_NAME} "Linux" LINUX)

IF(${LINUX})
#  set(CMAKE_CXX_COMPILER "clang++")
  set(CMAKE_BUILD_TYPE Release)
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -std=c++0x -march=corei7-avx -mtune=corei7-avx -flto -funroll-loops -fpermissive -save-temps -g")

  add_library(
    ipcm_lib
    STATIC
    EXCLUDE_FROM_ALL
    /home/espeholt/ipcm/msr.o
    /home/espeholt/ipcm/pci.o
    /home/espeholt/ipcm/cpucounters.o
  )

  set_target_properties(
    ipcm_lib
    PROPERTIES
    LINKER_LANGUAGE CXX
  )
ENDIF(${LINUX})

IF(APPLE)
  set(CMAKE_CXX_COMPILER "clang++")
  set(CMAKE_BUILD_TYPE Release)
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -arch x86_64 -flto -fasm-blocks -funroll-loops")
ENDIF(APPLE)

add_executable(Project2 main.cpp
                        matrix.h
                        layouts.h
                        matrixmul.h
                        stackallocator.h
                        test.h)

IF(UNIX)
    target_link_libraries(Project2 pthread)
ENDIF(UNIX)

IF(${LINUX})
  target_link_libraries(Project2 ipcm_lib rt)
ENDIF(${LINUX})
