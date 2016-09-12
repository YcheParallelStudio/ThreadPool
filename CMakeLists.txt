cmake_minimum_required(VERSION 3.4)
project(ThreadPool)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g")

add_executable(Demo example.cpp ThreadPool.h)
find_package(Threads)
target_link_libraries(Demo ${CMAKE_THREAD_LIBS_INIT})