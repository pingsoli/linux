#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include "pipe_ipc.h"

/*
void read_from_pipe(int fd)
{
  FILE *stream;
  int c;
  stream = fdopen(fd, "r");
  while ((c = fgetc(stream)) != EOF)
    putchar(c);

  fclose(stream);
}

void write_to_pipe(int fd, char *buf)
{
  FILE *stream;
  stream = fdopen(fd, "w");
  fprintf(stream, "%s", buf);
  fclose(stream);
}
*/

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
