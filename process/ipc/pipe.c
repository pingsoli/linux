/*
 * half-duplex pipe
 * Fork a child process and write some data to parent,
 * parent process receive the data and print to screen.
 *
 * message is only one direction: child -> parent
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
  int n, fds[2];
  pid_t pid;
  unsigned const int max_line = 1024;
  char line[max_line];

  pid = fork();

  if (pid < 0) {
    perror("fork error");
    exit(1);
  }

  if (pid == 0) { /* Child process */
    close (fds[0]);
    write(fds[1], "hello\n", 6);
  } else {        /* Parent process */
    close (fds[1]);
    n = read(fds[0], line, max_line);
    write(STDOUT_FILENO, line, n);
  }

  return 0;
}
