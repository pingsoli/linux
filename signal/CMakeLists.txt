cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED on)

################################################################################
add_executable(signal_first_program signal_first_program.c)
add_executable(sigaction            sigaction.c)
add_executable(block_signal         block_signal.c)
add_executable(sigprocmask          sigprocmask.c)
add_executable(sigpending           sigpending.c)
