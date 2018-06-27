cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)

# Signals
add_executable(signal_v1 signal_v1.c)
add_executable(signal_v2 signal_v2.c)

# Pipes
add_executable(pipe                        pipe.c)
add_executable(pipe_full_duplex            pipe_full_duplex.c)
add_executable(pipe_simulation_in_terminal pipe_simulation_in_terminal.c)
