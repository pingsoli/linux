#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

/*
 * Half-Duple Pipe
 * Child process send some data to parent process.
 * Pipe is a pair of file descriptors, the first is used
 * to reading, and the next is writing.
 *
 * Child process forked by parent process, and will inherient
 * the opend file descriptors, so we could close the unused
 * file descriptors.
 *
 * Pipe types: Half-Duplex Pipe and Full-Duplex Pipe
 *
 * Half-Duplex Pipe all one direction of data flow.
 * Full-Duplex Pipe allow two directions of the flowing.
 */

/*
 * Example 1
 * Fork a child process and write some data to parent,
 * parent process receive the data and print to screen.
 */
void half_pipe_exam1()
{
  int n, fds[2];
  pid_t pid;
  unsigned const int max_line = 1024;
  char line[max_line];

  pid = fork();

  if (pid < 0)
  {
    perror("fork error");
    exit(1);
  }

  if (pid == 0)
  {
    /* Child process */
    close (fds[0]);
    write(fds[1], "hello\n", 6);
  }
  else
  {
    /* Parent process */
    close (fds[1]);
    n = read(fds[0], line, max_line);
    write(STDOUT_FILENO, line, n);
  }
}

void pipe_test()
{
  int inpipe[2];
  int outpipe[2];

  /*
   * Pipe funciton create a pipe pair
   * pipe[0] for reading, pipe[1] for writing
   */
  pipe(inpipe);
  pipe(outpipe);

  const int arr_len = 4;
  char *buf = (char *) calloc(1, arr_len);
  strcpy(buf, "ABD");

  int x = fork();
  if (x == -1)
  {
    perror("fork error");
    exit(1);
  }

  if (x == 0)
  {
    // Chlid process
    char my_buf[arr_len];
    read(inpipe[0], my_buf, arr_len);

    my_buf[2] = 'C';
    write(outpipe[1], my_buf, arr_len);
    exit(0);
  }

  // Parent process
  char my_buf[4];
  write(inpipe[1], buf, arr_len);

  // Wait for data, blocking
  read(outpipe[0], my_buf, arr_len);

  // Release resources
  close(inpipe[0]);
  close(inpipe[1]);
  close(outpipe[0]);
  close(outpipe[1]);

  // Print changed info, ABD => ABC
  printf("%s\n", my_buf);

  return;
}

/*
 * Task: ls | wc -l
 * Attach stdout of 'ls' to the stdin of 'wc -l'
 * stdin -> ls -> wc -l -> stdout
 */
void
pipe_do_task()
{
  int pfds[2];

  pipe(pfds);

  int x = fork();
  if (x < 0)
  {
    perror("fork error");
    exit(1);
  }

  if (x == 0)
  {
    /* Chlid process */
    close(1);       /* Close stdout */
    dup(pfds[1]);   /* Make pfds[1] stdout */
    close(pfds[0]); /* No need this */
    execlp("ls", "ls", NULL);
  }
  else
  {
    /* Parent process */
    close(0);       /* Close stdin */
    dup(pfds[0]);   /* Make pfds[0] stdin */
    close(pfds[1]); /* No need this */
    execlp("wc", "wc", "-l", NULL);
  }

  return;
}

int main(int argc, char *argv[])
{
//  pipe_test();
//  pipe_do_task();
  half_pipe_exam1();

  return 0;
}
