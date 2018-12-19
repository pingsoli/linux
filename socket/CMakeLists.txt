cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)

add_executable(showip     showip.c)
add_executable(daemon     daemon.c)
add_executable(raw_socket raw_socket.c)
add_executable(conn_nonb conn_nonb.c)
add_executable(socket_is_writable socket_is_writable.c)

add_executable(multithread_bind multithread_bind.c)
target_link_libraries(multithread_bind -pthread)

add_executable(linger_option linger_option.c)
add_executable(keepalive_option keepalive_option.c)
add_executable(test_tcp_client test_tcp_client.c)
add_executable(test_tcp_server test_tcp_server.c)
