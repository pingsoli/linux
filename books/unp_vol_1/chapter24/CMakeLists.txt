cmake_minimum_required(VERSION 3.5)

add_executable(tcp_client_oob        tcp_client_oob.c)
add_executable(tcp_server_oob        tcp_server_oob.c)
add_executable(tcp_server_oob_select tcp_server_oob_select.c)
add_executable(tcp_server_oob_poll   tcp_server_oob_poll.c)
add_executable(tcp_server_sockatmark tcp_server_sockatmark.c)
