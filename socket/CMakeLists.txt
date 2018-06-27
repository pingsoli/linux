cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)

add_executable(showip     showip.c)
add_executable(daemon     daemon.c)
add_executable(raw_socket raw_socket.c)
