cmake_minimum_required(VERSION 3.5)

add_executable(hostent            hostent.c)
add_executable(daytime_client     daytime_client.c)
add_executable(daytime_client_r   daytime_client_r.c)
add_executable(tcp_daytime_client tcp_daytime_client.c)
add_executable(tcp_daytime_server tcp_daytime_server.c)
add_executable(udp_daytime_server udp_daytime_server.c)
add_executable(udp_daytime_client udp_daytime_client.c)
