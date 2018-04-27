cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED on)

################################################################################
add_executable(epoll_server epoll_server.c)
add_executable(poll_server poll_server.c)
add_executable(select_server select_server.c)
