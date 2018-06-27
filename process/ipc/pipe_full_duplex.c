/*
 * using two pipes to achive two data flow directions.
 * full duplex pipes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  int inpipe[2];
  int outpipe[2];

  pipe(inpipe);
  pipe(outpipe);

  const int arr_len = 4;
  char *buf = (char *) calloc(1, arr_len);
  strcpy(buf, "ABD");

  int pid = fork();
  if (pid == -1) {
    perror("fork error");
    exit(1);
  }

  if (pid == 0) { /* child process */
    char my_buf[arr_len];
    read(inpipe[0], my_buf, arr_len);

    my_buf[2] = 'C';
    write(outpipe[1], my_buf, arr_len);
    exit(0);
  }

  /* Parent process */

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

  return 0;
}
