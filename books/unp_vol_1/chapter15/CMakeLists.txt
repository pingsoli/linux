cmake_minimum_required(VERSION 3.5)

add_executable(unixbind           unixbind.c)
add_executable(unix_stream_server unix_stream_server.c)
add_executable(unix_stream_client unix_stream_client.c)
add_executable(unix_dgram_server  unix_dgram_server.c)
add_executable(unix_dgram_client  unix_dgram_client.c)
add_executable(mycat              mycat.c)
add_executable(openfile           openfile.c)
