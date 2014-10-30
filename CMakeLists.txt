cmake_minimum_required(VERSION 2.8.4)
project(Cache2)

set(MY_COMPILE_FLAGS "-std=c++11")

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${MY_COMPILE_FLAGS}")

set(SOURCE_FILES main.cpp)
add_executable(Cache2 ${SOURCE_FILES})