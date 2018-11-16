cmake_minimum_required(VERSION 3.12)
project(Heaps)

set(CMAKE_CXX_STANDARD 17)

add_executable(Heaps main.cpp src/FibonacciHeap.h src/Vector.h test/FibonacciHeapTest.cpp test/VectorTest.cpp)

add_subdirectory(lib/googletest-master)
include_directories(lib/googletest-master/googletest/include)
include_directories(lib/googletest-master/googlemock/include)
target_link_libraries(Heaps gtest gtest_main)