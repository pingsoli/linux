cmake_minimum_required(VERSION 3.5)

add_executable(tcp_echo_server_threads tcp_echo_server_threads.c)
target_link_libraries(tcp_echo_server_threads -pthread)

add_executable(tcp_echo_client_threads tcp_echo_client_threads.c)
target_link_libraries(tcp_echo_client_threads -pthread)

add_executable(mutex mutex.c)
target_link_libraries(mutex -pthread)
