cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)

add_executable(pipe_ipc pipe_ipc.c)
add_executable(signal_v1 signal_v1.c)
add_executable(signal_v2 signal_v2.c)
