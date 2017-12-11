#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * Simple set terminal and get propertities
 */
void set_and_get_terminal_prop()
{
  struct termios term;

  if (tcgetattr(STDIN_FILENO, &term) < 0)
  {
    fprintf(stderr, "tcgetattr failed\n");
    exit(1);
  }

  switch (term.c_cflag & CSIZE)
  {
  case CS5: 
    printf("5 bits/byte\n");
    break;

  case CS6:
    printf("6 bits/byte\n");
    break;

  case CS7:
    printf("7 bits/byte\n");
    break;

  case CS8:
    printf("8 bits/byte\n");
    break;

  default:
    printf("unknown bits/byte\n");
  }
}

/*
 * Usually print /dev/tty 
 *
 * 4 paltforms get the same output,
 */
void print_tty_name()
{
  printf("TTY name: %s\n", ctermid(NULL));
}

int is_a_tty(int fd)
{
  return isatty(fd) ? 1 : 0;
}

const char* get_tty_name(int fd)
{
  if (is_a_tty(fd))
    return ttyname(fd);
  else 
    return NULL;
}

/*
 * ./main </etc/passwd 2>/dev/null
 * 
 * output:
 * fd 0: not tty
 * fd 1: tty
 * fd 2: not tty
 */
void print_stdio_tty()
{
  printf("fd 0: %s\n", isatty(0) ? "tty" : "not a tty");
  printf("fd 1: %s\n", isatty(1) ? "tty" : "not a tty");
  printf("fd 2: %s\n", isatty(2) ? "tty" : "not a tty");
}

void print_std_tty_name()
{
  int fd = 0;

  for (fd = 0; fd < 3; fd++)
  {
    if (is_a_tty(fd))
      printf("fd %d: %s\n", fd, get_tty_name(fd));
    else 
      printf("fd %d: %s\n", fd, "is not tty");
  }
}
