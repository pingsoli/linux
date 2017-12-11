#pragma once

void set_and_get_terminal_prop();

void print_tty_name();

const char* get_tty_name(int fd);

void print_stdio_tty();

void print_std_tty_name();
