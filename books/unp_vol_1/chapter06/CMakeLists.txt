cmake_minimum_required(VERSION 3.5)

add_executable(tcp_echo_client_select tcp_echo_client_select.c)
add_executable(tcp_echo_server_select tcp_echo_server_select.c)
add_executable(tcp_echo_server_poll   tcp_echo_server_poll.c)
