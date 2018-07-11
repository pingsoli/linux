cmake_minimum_required(VERSION 3.5)

add_executable(udp_echo_server                   udp_echo_server.c)
add_executable(udp_echo_client                   udp_echo_client.c)
add_executable(udp_echo_client_lack_flow_control udp_echo_client_lack_flow_control.c)
add_executable(udp_outgoing_interface            udp_outgoing_interface.c)
add_executable(tcp_udp_echo_server_select        tcp_udp_echo_server_select.c)
