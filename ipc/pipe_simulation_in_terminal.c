/*
 * ls | wc -l
 * ls write the result to wc and output.
 * we write code to achieve the above function using pipe function.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv)
{
  int fds[2], childpid;

  pipe(fds);

  if ((childpid = fork()) < 0) {
    fprintf(stderr, "fork failed\n");
    exit(-1);
  }

  /*
   * child and parent processes share same stdin and stdout.
   * child process(ls) write output to pipe[1].
   */
  if (childpid == 0) { /* child process */
    dup2(fds[1], 1);   /* reuse stdout, all output will redirect to fds[1] */
    close(fds[0]);     /* no need this for read */
    execlp("ls", "ls", NULL);
  } else {             /* parent process */
    dup2(fds[0], 0);   /* reuse stdin, all stdin will redirect to fds[0] */
    close(fds[1]);     /* no need this for write */
    execlp("wc", "wc", "-l", NULL);
  }

  return 0;
}
