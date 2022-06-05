cmake_minimum_required(VERSION 3.20)
project(EX3 C)

set(CMAKE_C_STANDARD 99)
set(GCC_COVERAGE_COMPILE_FLAGS "-pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
add_executable(EX3 main.c boundedQueue.c unboundedQueue.c players.c players.h)
